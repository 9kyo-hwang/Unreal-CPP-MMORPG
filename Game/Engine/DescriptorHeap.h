#pragma once

class FSwapChain;

class FDescriptorHeap  // ���ſ��� View��� �̸����� ���
{
public:
	void Initialize(ComPtr<ID3D12Device> Device, shared_ptr<FSwapChain> InSwapChain);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(int32 Index) { assert(0 <= Index && Index < SWAP_CHAIN_BUFFER_COUNT); return RTVHandle[Index]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView();

private:
	// ����� RenderTargetView�� ���� �����ۿ� ������, ���� �߰��� ����
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	uint32 RTVHeapSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle[SWAP_CHAIN_BUFFER_COUNT]{};

	shared_ptr<FSwapChain> SwapChain;
};

