#pragma once

enum class EMultipleRenderTargetType : uint8
{
	SwapChain,	// BackBuffer, FrontBuffer
	GeometryBuffer,	// Position, Normal, Color. 디퍼드 셰이딩에서 사용
	Lighting,	// Diffuse, Specular
	END
};

constexpr int32 NumRenderTargetGeometryBufferMember = 3;
constexpr int32 NumRenderTargetLightingMember = 2;
constexpr uint8 NumMultipleRenderTarget = static_cast<uint8>(EMultipleRenderTargetType::END);

struct FRenderTarget
{
	shared_ptr<class FTexture> Target;	// 그릴 대상
	float ClearColor[4];	// 초기값 색상
};

class MultipleRenderTarget
{
public:
	void Create(EMultipleRenderTargetType InType, vector<FRenderTarget>& RenderTargets, shared_ptr<FTexture> InDepthStencilTexture);

	void OMSetRenderTargets(uint32 NumDescriptors, uint32 DescriptorHeapOffset) const;
	void OMSetRenderTargets() const;

	void ClearRenderTargetView(uint32 Index) const;
	void ClearRenderTargetView() const;

	shared_ptr<FTexture> GetRenderTargetTexture(uint32 Index) { return Data[Index].Target; }
	shared_ptr<FTexture> GetDepthStencilTexture() { return DepthStencilTexture; }

	void WaitRenderTargetToResource() const;
	void WaitResourceToRenderTarget() const;

private:
	EMultipleRenderTargetType Type;
	vector<FRenderTarget> Data;
	uint32 Num;
	shared_ptr<FTexture> DepthStencilTexture;
	ComPtr<ID3D12DescriptorHeap> RenderTargetDescriptorHeap;

private:
	uint32 RenderTargetDescriptorHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptorHeapStart;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptorHeapStart;

private:
	D3D12_RESOURCE_BARRIER RenderTargetToResource[8];
	D3D12_RESOURCE_BARRIER ResourceToRenderTarget[8];
};

