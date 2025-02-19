#pragma once

class FSwapChain;

class FDescriptorHeap  // 과거에는 View라는 이름으로 사용
{
public:
	void Initialize(ComPtr<ID3D12Device> Device, shared_ptr<FSwapChain> InSwapChain);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(int32 Index) { assert(0 <= Index && Index < SWAP_CHAIN_BUFFER_COUNT); return RTVHandle[Index]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView();

private:
	// 현재는 RenderTargetView에 대한 정보밖에 없으나, 추후 추가될 예정
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	uint32 RTVHeapSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle[SWAP_CHAIN_BUFFER_COUNT]{};

	shared_ptr<FSwapChain> SwapChain;
};

