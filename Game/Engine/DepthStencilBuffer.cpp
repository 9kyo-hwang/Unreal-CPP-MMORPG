#include "pch.h"
#include "DepthStencilBuffer.h"

#include "Engine.h"

void FDepthStencilBuffer::Initialize(const FWindowInfo& Window, DXGI_FORMAT InFormat)
{
	Format = InFormat;

	// Buffer
	D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(Format, Window.Width, Window.Height);
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE OptimizedClearValue = CD3DX12_CLEAR_VALUE(Format, 1.f, 0);  // 버퍼 초기화 시 세팅할 기본값
	DEVICE->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&OptimizedClearValue,
		IID_PPV_ARGS(&Resource)
	);

	// Descriptor Heap(View List. 1개만 있어도 반드시 Heap을 생성)
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		.NumDescriptors = 1,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
	};
	DEVICE->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescriptorHeap));

	// Descriptor(View). 첫 번째 인자를 가리키는 과정
	View = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	DEVICE->CreateDepthStencilView(Resource.Get(), nullptr, View);
}
