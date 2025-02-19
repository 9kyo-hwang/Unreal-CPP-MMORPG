#include "pch.h"
#include "DescriptorHeap.h"

#include "SwapChain.h"

void FDescriptorHeap::Initialize(ComPtr<ID3D12Device> Device, shared_ptr<FSwapChain> InSwapChain)
{
	SwapChain = InSwapChain;

	/*
	 *	Descriptor Heap으로 RTV 생성
	 *	DX11까지는 아래 목록을 각각 관리, DX12부터 통합으로 DescriptorHeap에서 관리
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
	 *	같은 종류 데이터끼리 묶어서 배열(힙)로 관리
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
	// SwapChain: Resource 그 자체(Front/Back Buffer)
	// 기안서인 View(Descriptor)를 GPU에 넘겨줘야 함
	return GetRTV(SwapChain->GetCurrentBackBufferIndex());
}
