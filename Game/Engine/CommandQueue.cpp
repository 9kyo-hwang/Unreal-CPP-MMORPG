#include "pch.h"
#include "CommandQueue.h"

#include "DescriptorHeap.h"
#include "SwapChain.h"

FCommandQueue::~FCommandQueue()
{
	::CloseHandle(FenceEvent);
}

void FCommandQueue::Initialize(ComPtr<ID3D12Device> Device, shared_ptr<FSwapChain> InSwapChain,
                               shared_ptr<FDescriptorHeap> InDescriptorHeap)
{
	SwapChain = InSwapChain;
	DescriptorHeap = InDescriptorHeap;

	// GPU가 직접 실행하는 커맨드 목록
	constexpr D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	D3D12_COMMAND_QUEUE_DESC Desc
	{
		.Type = Type,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
	};

	Device->CreateCommandQueue(&Desc, IID_PPV_ARGS(&CommandQueue));
	Device->CreateCommandAllocator(Type, IID_PPV_ARGS(&CommandAllocator));
	/*
	 *	GPU가 하나인 시스템에서는 0
	 *	DIRECT or BUNDLE
	 *	Allocator
	 *	초기 상태(그리기 명령은 nullptr 지정)
	 */
	Device->CreateCommandList(0, Type, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList));

	/*
	 *	상태 2가지: Close / Open
	 *	Open 상태에서는 Command Push, Close 상태에서는 Command Pop
	 */
	CommandList->Close();

	Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
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

	D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		SwapChain->GetCurrentBackBufferResource().Get(), 
		D3D12_RESOURCE_STATE_PRESENT,	// 화면 출력(Before). FrontBuffer -> BackBuffer Swap
		D3D12_RESOURCE_STATE_RENDER_TARGET	// 외주 결과물(After)
	);
	CommandList->ResourceBarrier(1, &Barrier);

	CommandList->RSSetViewports(1, Viewport);
	CommandList->RSSetScissorRects(1, Rect);

	// GPU가 사용할(결과물을 그려야할) 백 버퍼를 지정
	D3D12_CPU_DESCRIPTOR_HANDLE BackBufferView = DescriptorHeap->GetBackBufferView();
	CommandList->ClearRenderTargetView(BackBufferView, Colors::LightSteelBlue, 0, nullptr);
	CommandList->OMSetRenderTargets(1, &BackBufferView, false, nullptr);
}

void FCommandQueue::RenderEnd()
{
	D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		SwapChain->GetCurrentBackBufferResource().Get(),
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
