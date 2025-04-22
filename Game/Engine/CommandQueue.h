#pragma once

class FSwapChain;

class FGraphicsCommandQueue
{
public:
	FGraphicsCommandQueue();
	~FGraphicsCommandQueue();

	void Initialize(ComPtr<ID3D12Device> Device, TSharedPtr<FSwapChain> InSwapChain);
	void WaitSync();

	void PreRender();
	void PostRender();

	// ResourceCommandQueue에 있는 모든 작업을 다 처리하는 함수
	void FlushResources();

	ComPtr<ID3D12CommandQueue> GetCommandQueue() { return CommandQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return CommandList; }
	ComPtr<ID3D12GraphicsCommandList> GetResourceCommandList() { return ResourceCommandList; }

private:
	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;

	ComPtr<ID3D12CommandAllocator> ResourceCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> ResourceCommandList;

	// CPU - GPU 동기화를 위해 필요한 변수들
	ComPtr<ID3D12Fence> Fence;
	uint32 FenceValue;
	HANDLE FenceEvent;

	TSharedPtr<FSwapChain> SwapChain;
};

class FComputeCommandQueue
{
public:
	FComputeCommandQueue();
	~FComputeCommandQueue();

	void Initialize(ComPtr<ID3D12Device> Device);
	void WaitSync();
	void Flush();

	ComPtr<ID3D12CommandQueue> GetCommandQueue() { return CommandQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return CommandList; }

private:
	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;

	ComPtr<ID3D12Fence> Fence;
	uint32 FenceValue;
	HANDLE FenceEvent;
};