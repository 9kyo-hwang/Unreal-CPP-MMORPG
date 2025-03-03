#pragma once
#include "EnginePCH.h"

class FSwapChain
{
public:
	void Initialize(const FWindowInfo& Info, ComPtr<IDXGIFactory> DXGI, ComPtr<ID3D12CommandQueue> CommandQueue);
	void Present() const;
	void SwapIndex();

	ComPtr<IDXGISwapChain> GetSwapChain() { return SwapChain; }
	ComPtr<ID3D12Resource> GetRenderTarget(int32 Index) { return RenderTargets[Index]; }
	ComPtr<ID3D12Resource> GetCurrentBackBufferResource() { return RenderTargets[BackBufferIndex]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() { return RenderTargetViewHandle[BackBufferIndex]; }

private:
	void CreateSwapChain(const FWindowInfo& Info, ComPtr<IDXGIFactory> DXGI, ComPtr<ID3D12CommandQueue> CommandQueue);
	void CreateRenderTargetView();

private:
	ComPtr<IDXGISwapChain> SwapChain;
	ComPtr<ID3D12Resource> RenderTargets[SWAP_CHAIN_BUFFER_COUNT];
	ComPtr<ID3D12DescriptorHeap> RenderTargetViewHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandle[SWAP_CHAIN_BUFFER_COUNT]{};
	uint32 BackBufferIndex = 0;
};

