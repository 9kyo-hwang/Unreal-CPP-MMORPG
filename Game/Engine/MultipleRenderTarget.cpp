#include "pch.h"
#include "MultipleRenderTarget.h"

#include "Engine.h"
#include "Texture.h"

void MultipleRenderTarget::Create(EMultipleRenderTargetType InType, vector<FRenderTarget>& RenderTargets,
                                  shared_ptr<FTexture> InDepthStencilTexture)
{
	Type = InType;
	Data = RenderTargets;
	Num = static_cast<uint32>(RenderTargets.size());
	DepthStencilTexture = InDepthStencilTexture;

	// Render Target 벡터를 설명하는 디스크립터
	D3D12_DESCRIPTOR_HEAP_DESC Desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.NumDescriptors = Num,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0,
	};

	DEVICE->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&RenderTargetDescriptorHeap));

	RenderTargetDescriptorHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	RenderTargetDescriptorHeapStart = RenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	DepthStencilDescriptorHeapStart = DepthStencilTexture->GetDepthStencilDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	for (uint32 Index = 0; Index < Num; ++Index)
	{
		uint32 DestSize = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE DestHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RenderTargetDescriptorHeapStart, Index * RenderTargetDescriptorHeapSize);

		uint32 SrcSize = 1;
		ComPtr<ID3D12DescriptorHeap> SrcRenderTargetDescriptorHeapStart = Data[Index].Target->GetRenderTargetDescriptorHeap();
		D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle = SrcRenderTargetDescriptorHeapStart->GetCPUDescriptorHandleForHeapStart();

		DEVICE->CopyDescriptors(1, &DestHandle, &DestSize, 1, &SrcHandle, &SrcSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
}

void MultipleRenderTarget::OMSetRenderTargets(uint32 NumRenderTargetDescriptors, uint32 RenderTargetDescriptorHeapOffset) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RenderTargetDescriptorHeapStart, RenderTargetDescriptorHeapOffset * RenderTargetDescriptorHeapSize);
	COMMAND_LIST->OMSetRenderTargets(NumRenderTargetDescriptors, &RenderTargetDescriptorHandle, false, &DepthStencilDescriptorHeapStart);
}

void MultipleRenderTarget::OMSetRenderTargets() const
{
	COMMAND_LIST->OMSetRenderTargets(Num, &RenderTargetDescriptorHeapStart, true, &DepthStencilDescriptorHeapStart);
}

void MultipleRenderTarget::ClearRenderTargetView(uint32 Index) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RenderTargetDescriptorHeapStart, Index * RenderTargetDescriptorHeapSize);
	COMMAND_LIST->ClearRenderTargetView(RenderTargetDescriptorHandle, Data[Index].ClearColor, 0, nullptr);
}

void MultipleRenderTarget::ClearRenderTargetView() const
{
	for (uint32 Index = 0; Index < Num; ++Index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RenderTargetDescriptorHeapStart, Index * RenderTargetDescriptorHeapSize);
		COMMAND_LIST->ClearRenderTargetView(RenderTargetDescriptorHandle, Data[Index].ClearColor, 0, nullptr);
	}

	COMMAND_LIST->ClearDepthStencilView(DepthStencilDescriptorHeapStart, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}