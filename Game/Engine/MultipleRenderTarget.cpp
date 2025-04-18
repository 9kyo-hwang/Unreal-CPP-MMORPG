#include "pch.h"
#include "MultipleRenderTarget.h"

#include "Engine.h"
#include "Texture.h"

MultipleRenderTarget::MultipleRenderTarget()
	: Type()
	, Num(0)
	, RenderTargetDescriptorIncrementSize(0)
	, RenderTargetDescriptorHeapStart()
	, DepthStencilDescriptorHeapStart()
	, RenderTargetToResourceBarriers{}
	, ResourceToRenderTargetBarriers{}
{
}

MultipleRenderTarget::~MultipleRenderTarget() = default;

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

	RenderTargetDescriptorIncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	RenderTargetDescriptorHeapStart = RenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	DepthStencilDescriptorHeapStart = DepthStencilTexture->GetDepthStencilDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	for (uint32 Index = 0; Index < Num; ++Index)
	{
		uint32 DestSize = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE DestHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RenderTargetDescriptorHeapStart, Index * RenderTargetDescriptorIncrementSize);

		uint32 SrcSize = 1;
		ComPtr<ID3D12DescriptorHeap> SrcRenderTargetDescriptorHeapStart = Data[Index].Target->GetRenderTargetDescriptorHeap();

		D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle = SrcRenderTargetDescriptorHeapStart->GetCPUDescriptorHandleForHeapStart();

		DEVICE->CopyDescriptors(1, &DestHandle, &DestSize, 1, &SrcHandle, &SrcSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// CommandQueue의 RenderBegin에서 PRESENT <-> RENDER_TARGET 하는 것과 같은 상황
	// GeometryBuffer와 Lighting에서 사용할 예정
	for (uint32 Index = 0; Index < Num; ++Index)
	{
		RenderTargetToResourceBarriers[Index] = CD3DX12_RESOURCE_BARRIER::Transition(
			Data[Index].Target->GetTexture2D().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATE_COMMON
		);

		ResourceToRenderTargetBarriers[Index] = CD3DX12_RESOURCE_BARRIER::Transition(
			Data[Index].Target->GetTexture2D().Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
	}
}

void MultipleRenderTarget::OMSetRenderTargets(uint32 NumDescriptors, uint32 DescriptorHeapOffset) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RenderTargetDescriptorHeapStart, DescriptorHeapOffset * RenderTargetDescriptorIncrementSize);
	GRAPHICS_COMMAND_LIST->OMSetRenderTargets(NumDescriptors, &DescriptorHandle, false, &DepthStencilDescriptorHeapStart);
}

void MultipleRenderTarget::OMSetRenderTargets() const
{
	GRAPHICS_COMMAND_LIST->OMSetRenderTargets(Num, &RenderTargetDescriptorHeapStart, true, &DepthStencilDescriptorHeapStart);
}

void MultipleRenderTarget::ClearRenderTargetView(uint32 Index) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RenderTargetDescriptorHeapStart, Index * RenderTargetDescriptorIncrementSize);
	GRAPHICS_COMMAND_LIST->ClearRenderTargetView(DescriptorHandle, Data[Index].ClearColor, 0, nullptr);
	GRAPHICS_COMMAND_LIST->ClearDepthStencilView(DepthStencilDescriptorHeapStart, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void MultipleRenderTarget::ClearRenderTargetView() const
{
	WaitForUseAsRenderTarget();

	for (uint32 Index = 0; Index < Num; ++Index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RenderTargetDescriptorHeapStart, Index * RenderTargetDescriptorIncrementSize);
		GRAPHICS_COMMAND_LIST->ClearRenderTargetView(DescriptorHandle, Data[Index].ClearColor, 0, nullptr);
	}

	GRAPHICS_COMMAND_LIST->ClearDepthStencilView(DepthStencilDescriptorHeapStart, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void MultipleRenderTarget::WaitForUseAsAResource() const
{
	GRAPHICS_COMMAND_LIST->ResourceBarrier(Num, RenderTargetToResourceBarriers);
}

void MultipleRenderTarget::WaitForUseAsRenderTarget() const
{
	GRAPHICS_COMMAND_LIST->ResourceBarrier(Num, ResourceToRenderTargetBarriers);
}
