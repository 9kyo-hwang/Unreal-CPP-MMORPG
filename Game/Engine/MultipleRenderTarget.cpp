#include "pch.h"
#include "MultipleRenderTarget.h"

#include "Engine.h"
#include "Texture.h"

FMultipleRenderTarget::FMultipleRenderTarget()
	: Type()
	, Num(0)
	, RTVIncrementSize(0)
	, RTVHeapStart()
	, DSVHeapStart()
	, RenderTargetToResourceBarriers{}
	, ResourceToRenderTargetBarriers{}
{
}

FMultipleRenderTarget::~FMultipleRenderTarget() = default;

void FMultipleRenderTarget::Create(ComPtr<ID3D12Device> Device, ERenderTargetType InType, vector<FRenderTarget>& InRenderTargets,
                                  TSharedPtr<FTexture> InDepthStencilTexture)
{
	Type = InType;
	RenderTargets = InRenderTargets;
	Num = StaticCast<uint32>(InRenderTargets.size());
	DepthStencilTexture = InDepthStencilTexture;

	// Render Target 벡터를 설명하는 디스크립터
	D3D12_DESCRIPTOR_HEAP_DESC Desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.NumDescriptors = Num,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0,
	};
	Device->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&RTVHeap));

	RTVIncrementSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	RTVHeapStart = RTVHeap->GetCPUDescriptorHandleForHeapStart();
	DSVHeapStart = DepthStencilTexture->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();

	for (uint32 Index = 0; Index < Num; ++Index)
	{
		uint32 DestSize = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE DestHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeapStart, Index * RTVIncrementSize);

		uint32 SrcSize = 1;
		ComPtr<ID3D12DescriptorHeap> SrcRenderTargetDescriptorHeapStart = RenderTargets[Index].Texture->GetRTVHeap();
		D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle = SrcRenderTargetDescriptorHeapStart->GetCPUDescriptorHandleForHeapStart();

		Device->CopyDescriptors(1, &DestHandle, &DestSize, 1, &SrcHandle, &SrcSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// CommandQueue의 RenderBegin에서 PRESENT <-> RENDER_TARGET 하는 것과 같은 상황
	// GeometryBuffer와 Lighting에서 사용할 예정
	for (uint32 Index = 0; Index < Num; ++Index)
	{
		RenderTargetToResourceBarriers[Index] = CD3DX12_RESOURCE_BARRIER::Transition(
			RenderTargets[Index].Texture->GetTexture2D().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATE_COMMON
		);

		ResourceToRenderTargetBarriers[Index] = CD3DX12_RESOURCE_BARRIER::Transition(
			RenderTargets[Index].Texture->GetTexture2D().Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
	}
}

void FMultipleRenderTarget::OMSetRenderTargets(uint32 NumViews, uint32 HeapOffset) const
{
	// Shadow가 생기면서 모든 렌더 타겟의 크기가 동일하지 않음 -> Viewport와 Rect를 새로 계산해줘야 함
	D3D12_VIEWPORT Viewport = D3D12_VIEWPORT{ 0.f, 0.f, RenderTargets[0].Texture->GetWidth(), RenderTargets[0].Texture->GetHeight(), 0.f, 1.f };
	D3D12_RECT Rect = D3D12_RECT{ 0, 0, StaticCast<LONG>(RenderTargets[0].Texture->GetWidth()), StaticCast<LONG>(RenderTargets[0].Texture->GetHeight()) };

	GRAPHICS_COMMAND_LIST->RSSetViewports(1, &Viewport);
	GRAPHICS_COMMAND_LIST->RSSetScissorRects(1, &Rect);

	D3D12_CPU_DESCRIPTOR_HANDLE View = CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeapStart, HeapOffset * RTVIncrementSize);
	GRAPHICS_COMMAND_LIST->OMSetRenderTargets(NumViews, &View, false, &DSVHeapStart);
}

void FMultipleRenderTarget::OMSetRenderTargets() const
{
	D3D12_VIEWPORT Viewport = D3D12_VIEWPORT{ 0.f, 0.f, RenderTargets[0].Texture->GetWidth(), RenderTargets[0].Texture->GetHeight(), 0.f, 1.f };
	D3D12_RECT Rect = D3D12_RECT{ 0, 0, StaticCast<LONG>(RenderTargets[0].Texture->GetWidth()), StaticCast<LONG>(RenderTargets[0].Texture->GetHeight()) };

	GRAPHICS_COMMAND_LIST->RSSetViewports(1, &Viewport);
	GRAPHICS_COMMAND_LIST->RSSetScissorRects(1, &Rect);
	GRAPHICS_COMMAND_LIST->OMSetRenderTargets(Num, &RTVHeapStart, true, &DSVHeapStart);
}

void FMultipleRenderTarget::ClearRTV(uint32 Index) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE View = CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeapStart, Index * RTVIncrementSize);
	GRAPHICS_COMMAND_LIST->ClearRenderTargetView(View, RenderTargets[Index].ClearColor, 0, nullptr);
	GRAPHICS_COMMAND_LIST->ClearDepthStencilView(DSVHeapStart, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void FMultipleRenderTarget::ClearRTV() const
{
	WaitForUseAsRenderTarget();

	for (uint32 Index = 0; Index < Num; ++Index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE View = CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeapStart, Index * RTVIncrementSize);
		GRAPHICS_COMMAND_LIST->ClearRenderTargetView(View, RenderTargets[Index].ClearColor, 0, nullptr);
	}

	GRAPHICS_COMMAND_LIST->ClearDepthStencilView(DSVHeapStart, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void FMultipleRenderTarget::WaitForUseAsAResource() const
{
	GRAPHICS_COMMAND_LIST->ResourceBarrier(Num, RenderTargetToResourceBarriers);
}

void FMultipleRenderTarget::WaitForUseAsRenderTarget() const
{
	GRAPHICS_COMMAND_LIST->ResourceBarrier(Num, ResourceToRenderTargetBarriers);
}
