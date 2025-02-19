#include "pch.h"
#include "DescriptorHeap.h"

#include "SwapChain.h"

void FDescriptorHeap::Initialize(ComPtr<ID3D12Device> Device, shared_ptr<FSwapChain> InSwapChain)
{
	SwapChain = InSwapChain;

	/*
	 *	Descriptor Heap���� RTV ����
	 *	DX11������ �Ʒ� ����� ���� ����, DX12���� �������� DescriptorHeap���� ����
	 *	RTV(Render Target View), DSV(Depth Stencil View), CBV(Constant Buffer View), SRV(Shader Resource View), UAV(Unordered Access View)
	 */

	RTVHeapSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC RTVDesc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0
	};
	/*
	 *	���� ���� �����ͳ��� ��� �迭(��)�� ����
	 *	RTV: [ ] [ ]
	 *	DSV: [ ] [ ]
	 *	...
	 */
	Device->CreateDescriptorHeap(&RTVDesc, IID_PPV_ARGS(&RTVHeap));

	D3D12_CPU_DESCRIPTOR_HANDLE RTVHeapBegin = RTVHeap->GetCPUDescriptorHandleForHeapStart();
	for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		RTVHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeapBegin, i * RTVHeapSize);
		Device->CreateRenderTargetView(SwapChain->GetRenderTarget(i).Get(), nullptr, RTVHandle[i]);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE FDescriptorHeap::GetBackBufferView()
{
	// SwapChain: Resource �� ��ü(Front/Back Buffer)
	// ��ȼ��� View(Descriptor)�� GPU�� �Ѱ���� ��
	return GetRTV(SwapChain->GetCurrentBackBufferIndex());
}
