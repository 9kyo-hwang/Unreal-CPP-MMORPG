#pragma once
#include "EnginePCH.h"

class FSwapChain
{
public:
	void Initialize(const FWindowInfo& Info, ComPtr<IDXGIFactory> DXGI, ComPtr<ID3D12CommandQueue> CommandQueue);
	void Present() const;
	void SwapIndex();

	ComPtr<IDXGISwapChain> GetSwapChain() { return SwapChain; }
	ComPtr<ID3D12Resource> GetRenderTarget(int32 Index) { assert(0 <= Index && Index < SWAP_CHAIN_BUFFER_COUNT);  return RenderTargets[Index]; }

	uint32 GetCurrentBackBufferIndex() const { return BackBufferIndex; }
	ComPtr<ID3D12Resource> GetCurrentBackBufferResource() { return RenderTargets[BackBufferIndex]; }

private:
	ComPtr<IDXGISwapChain> SwapChain;
	ComPtr<ID3D12Resource> RenderTargets[SWAP_CHAIN_BUFFER_COUNT];
	uint32 BackBufferIndex = 0;
};

