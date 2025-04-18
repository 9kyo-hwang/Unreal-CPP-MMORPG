#pragma once
#include "EnginePCH.h"

class FSwapChain
{
public:
	FSwapChain();
	~FSwapChain();
	
	void Initialize(const FWindowInfo& Info, ComPtr<IDXGIFactory> DXGI, ComPtr<ID3D12CommandQueue> CommandQueue);
	void Present() const;
	void SwapIndex();

	ComPtr<IDXGISwapChain> GetDXGISwapChain() { return SwapChain; }
	uint32 GetBackBufferIndex() const { return BackBufferIndex; }

private:
	void Create(const FWindowInfo& Info, ComPtr<IDXGIFactory> DXGI, ComPtr<ID3D12CommandQueue> CommandQueue);

private:
	ComPtr<IDXGISwapChain> SwapChain;
	uint32 BackBufferIndex;
};

