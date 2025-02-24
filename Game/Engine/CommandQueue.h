#pragma once

class FSwapChain;

class FCommandQueue
{
public:
	~FCommandQueue();

	void Initialize(ComPtr<ID3D12Device> Device, shared_ptr<FSwapChain> InSwapChain);
	void WaitSync();

	void RenderBegin(const D3D12_VIEWPORT* Viewport, const D3D12_RECT* Rect);
	void RenderEnd();

	ComPtr<ID3D12CommandQueue> GetD3DCommandQueue() { return CommandQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetD3DCommandList() { return CommandList; }

private:
	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;

	// CPU - GPU 동기화를 위해 필요한 변수들
	ComPtr<ID3D12Fence> Fence;
	uint32 FenceValue = 0;
	HANDLE FenceEvent = INVALID_HANDLE_VALUE;

	shared_ptr<FSwapChain> SwapChain;
};

