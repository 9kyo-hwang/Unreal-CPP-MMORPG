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

	// GPU�� ���� �����ϴ� Ŀ�ǵ� ���
	constexpr D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	D3D12_COMMAND_QUEUE_DESC Desc
	{
		.Type = Type,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
	};

	Device->CreateCommandQueue(&Desc, IID_PPV_ARGS(&CommandQueue));
	Device->CreateCommandAllocator(Type, IID_PPV_ARGS(&CommandAllocator));
	/*
	 *	GPU�� �ϳ��� �ý��ۿ����� 0
	 *	DIRECT or BUNDLE
	 *	Allocator
	 *	�ʱ� ����(�׸��� ����� nullptr ����)
	 */
	Device->CreateCommandList(0, Type, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList));

	/*
	 *	���� 2����: Close / Open
	 *	Open ���¿����� Command Push, Close ���¿����� Command Pop
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
		Fence->SetEventOnCompletion(FenceValue, FenceEvent);  // �ش� Value�� �ϰ��� GPU�� ���´ٸ� Event Trigger
		::WaitForSingleObject(FenceEvent, INFINITE);  // Event�� �ߵ��Ǹ� ��� ���� CPU�� �۾� ����
	}
}

void FCommandQueue::RenderBegin(const D3D12_VIEWPORT* Viewport, const D3D12_RECT* Rect)
{
	CommandAllocator->Reset();
	CommandList->Reset(CommandAllocator.Get(), nullptr);

	D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		SwapChain->GetCurrentBackBufferResource().Get(), 
		D3D12_RESOURCE_STATE_PRESENT,	// ȭ�� ���(Before). FrontBuffer -> BackBuffer Swap
		D3D12_RESOURCE_STATE_RENDER_TARGET	// ���� �����(After)
	);
	CommandList->ResourceBarrier(1, &Barrier);

	CommandList->RSSetViewports(1, Viewport);
	CommandList->RSSetScissorRects(1, Rect);

	// GPU�� �����(������� �׷�����) �� ���۸� ����
	D3D12_CPU_DESCRIPTOR_HANDLE BackBufferView = DescriptorHeap->GetBackBufferView();
	CommandList->ClearRenderTargetView(BackBufferView, Colors::LightSteelBlue, 0, nullptr);
	CommandList->OMSetRenderTargets(1, &BackBufferView, false, nullptr);
}

void FCommandQueue::RenderEnd()
{
	D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		SwapChain->GetCurrentBackBufferResource().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,	// ���� �����(Before)
		D3D12_RESOURCE_STATE_PRESENT	// ȭ�� ���(After). BackBuffer -> FrontBuffer Swap
	);
	CommandList->ResourceBarrier(1, &Barrier);
	CommandList->Close();

	// Ŀ�ǵ� ��� ������ ����
	ID3D12CommandList* CommandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

	SwapChain->Present();

	WaitSync();

	SwapChain->SwapIndex();
}
