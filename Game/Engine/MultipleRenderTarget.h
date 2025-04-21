#pragma once

enum class ERenderTargetType : uint8
{
	SwapChain,	// BackBuffer, FrontBuffer
	GeometryBuffer,	// Position, Normal, Color. 디퍼드 셰이딩에서 사용
	Lighting,	// Diffuse, Specular
	END
};

constexpr int32 NumRenderTargetGeometryBufferMember = 3;
constexpr int32 NumRenderTargetLightingMember = 2;
constexpr uint8 NumRenderTargets = ConstexprCast<uint8>(ERenderTargetType::END);

struct FRenderTarget
{
	TSharedPtr<class FTexture> Texture;	// 그릴 대상
	float ClearColor[4];	// 초기값 색상
};

class MultipleRenderTarget
{
public:
	MultipleRenderTarget();
	~MultipleRenderTarget();
	
	void Create(ComPtr<ID3D12Device> Device, ERenderTargetType InType, vector<FRenderTarget>& InRenderTargets, TSharedPtr<FTexture> InDepthStencilTexture);

	void OMSetRenderTargets(uint32 NumViews, uint32 HeapOffset) const;
	void OMSetRenderTargets() const;

	void ClearRTV(uint32 Index) const;
	void ClearRTV() const;

	TSharedPtr<FTexture> GetRenderTargetTexture(uint32 Index) { return RenderTargets[Index].Texture; }
	TSharedPtr<FTexture> GetDepthStencilTexture() { return DepthStencilTexture; }

	void WaitForUseAsAResource() const;
	void WaitForUseAsRenderTarget() const;

private:
	ERenderTargetType Type;
	vector<FRenderTarget> RenderTargets;
	uint32 Num;
	TSharedPtr<FTexture> DepthStencilTexture;
	ComPtr<ID3D12DescriptorHeap> RTVHeap;

private:
	uint32 RTVIncrementSize;
	D3D12_CPU_DESCRIPTOR_HANDLE RTVHeapStart;
	D3D12_CPU_DESCRIPTOR_HANDLE DSVHeapStart;

private:
	D3D12_RESOURCE_BARRIER RenderTargetToResourceBarriers[8];
	D3D12_RESOURCE_BARRIER ResourceToRenderTargetBarriers[8];
};

