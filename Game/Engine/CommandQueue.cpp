#include "pch.h"
#include "CommandQueue.h"

#include "Engine.h"
#include "SwapChain.h"
#include "Texture.h"

FCommandQueue::~FCommandQueue()
{
	::CloseHandle(FenceEvent);
}

void FCommandQueue::Initialize(shared_ptr<FSwapChain> InSwapChain)
{
	SwapChain = InSwapChain;

	// GPU가 직접 실행하는 커맨드 목록
	constexpr D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	D3D12_COMMAND_QUEUE_DESC Desc
	{
		.Type = Type,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
	};

	DEVICE->CreateCommandQueue(&Desc, IID_PPV_ARGS(&CommandQueue));
	DEVICE->CreateCommandAllocator(Type, IID_PPV_ARGS(&CommandAllocator));
	/*
	 *	GPU가 하나인 시스템에서는 0
	 *	DIRECT or BUNDLE
	 *	Allocator
	 *	초기 상태(그리기 명령은 nullptr 지정)
	 */
	DEVICE->CreateCommandList(0, Type, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList));

	/*
	 *	상태 2가지: Close / Open
	 *	Open 상태에서는 Command Push, Close 상태에서는 Command Pop
	 */
	CommandList->Close();

	DEVICE->CreateCommandAllocator(Type, IID_PPV_ARGS(&ResourceCommandAllocator));
	DEVICE->CreateCommandList(0, Type, ResourceCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&ResourceCommandList));

	DEVICE->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
	FenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
}

void FCommandQueue::WaitSync()
{
	FenceValue++;
	CommandQueue->Signal(Fence.Get(), FenceValue);

	if (Fence->GetCompletedValue() < FenceValue)
	{
		Fence->SetEventOnCompletion(FenceValue, FenceEvent);  // 해당 Value의 일감을 GPU가 끝냈다면 Event Trigger
		::WaitForSingleObject(FenceEvent, INFINITE);  // Event가 발동되면 대기 중인 CPU가 작업 수행
	}
}

void FCommandQueue::RenderBegin(const D3D12_VIEWPORT* Viewport, const D3D12_RECT* Rect)
{
	CommandAllocator->Reset();
	CommandList->Reset(CommandAllocator.Get(), nullptr);

	int8 BackBufferIndex = SwapChain->GetBackBufferIndex();

	D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::SwapChain)->GetRenderTargetTexture(BackBufferIndex)->GetTexture2D().Get(),
		D3D12_RESOURCE_STATE_PRESENT,	// 화면 출력(Before). FrontBuffer -> BackBuffer Swap
		D3D12_RESOURCE_STATE_RENDER_TARGET	// 외주 결과물(After)
	);

	CommandList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());	// 루트 서명 추가

	GEngine->GetConstantBuffer(EConstantBufferType::Transform)->Clear();	// 여러 개의 상수 버퍼가 생기면서
	GEngine->GetConstantBuffer(EConstantBufferType::Material)->Clear();		// 각 타입 별 버퍼 모두 초기화

	GEngine->GetTableDescriptorHeap()->Clear();  // TableDescriptorHeap도 초기화

	/**
	 *	SetDescriptorHeaps는 프레임마다 단 한 번만 호출! 또한 힙은 최대 2개만 넣을 수 있음
	 *	이를 호출해야 TableDescriptorHeap에서 COMMAND_LIST->SetGraphicsRootDescriptor()가 동작, 없으면 Crash
	 */
	ID3D12DescriptorHeap* DescriptorHeap = GEngine->GetTableDescriptorHeap()->GetD3DDescriptorHeap().Get();
	CommandList->SetDescriptorHeaps(1, &DescriptorHeap);

	CommandList->ResourceBarrier(1, &Barrier);

	// CommandList가 Reset될 때를 대비해 설정
	CommandList->RSSetViewports(1, Viewport);
	CommandList->RSSetScissorRects(1, Rect);

	// NOTE: 원래 여기서 수행하던 ClearRenderTargetView/OMSetRenderTargets/ClearDepthStencilView는 Scene의 Render로 이관됨
}

void FCommandQueue::RenderEnd()
{
	int8 BackBufferIndex = SwapChain->GetBackBufferIndex();
	D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::SwapChain)->GetRenderTargetTexture(BackBufferIndex)->GetTexture2D().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,	// 외주 결과물(Before)
		D3D12_RESOURCE_STATE_PRESENT	// 화면 출력(After). BackBuffer -> FrontBuffer Swap
	);
	CommandList->ResourceBarrier(1, &Barrier);
	CommandList->Close();

	// 커맨드 목록 실제로 실행
	ID3D12CommandList* CommandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

	SwapChain->Present();

	WaitSync();

	SwapChain->SwapIndex();
}

void FCommandQueue::FlushResourceCommandQueue()
{
	ResourceCommandList->Close();

	ID3D12CommandList* CommandLists[ ] = { ResourceCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

	WaitSync();

	ResourceCommandAllocator->Reset();
	ResourceCommandList->Reset(ResourceCommandAllocator.Get(), nullptr);
}
