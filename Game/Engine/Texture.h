#pragma once
#include "Object.h"

class FTexture : public Object
{
	using Super = Object;

public:
	FTexture();
	~FTexture() override;

	void Load(const wstring& Path) override;

public:
	void Create(DXGI_FORMAT Format, uint32 Width, uint32 Height, const D3D12_HEAP_PROPERTIES& HeapProperties, 
		D3D12_HEAP_FLAGS HeapFlags, D3D12_RESOURCE_FLAGS ResourceFlags, FVector4 ClearColor = {});
	void Create(ComPtr<ID3D12Resource> InTexture2D);

public:
	ComPtr<ID3D12Resource> GetTexture2D() { return Texture2D; }
	ComPtr<ID3D12DescriptorHeap> GetShaderResourceDescriptorHeap() { return ShaderResourceDescriptorHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRenderTargetDescriptorHeap() { return RenderTargetDescriptorHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDepthStencilDescriptorHeap() { return DepthStencilDescriptorHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUnorderedAccessDescriptorHeap() { return UnorderedAccessDescriptorHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceDescriptorHandle() { return ShaderResourceDescriptorHeapStart; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessDescriptorHandle() { return UnorderedAccessDescriptorHeapStart; }

private:
	ScratchImage Image;
	ComPtr<ID3D12Resource> Texture2D;

	// ConstantBuffer는 View를 여러 개 담은 Heap을 사용하지만, Texture는 Heap에 View 하나만 사용
	ComPtr<ID3D12DescriptorHeap> ShaderResourceDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> RenderTargetDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> DepthStencilDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> UnorderedAccessDescriptorHeap;

private:
	D3D12_CPU_DESCRIPTOR_HANDLE ShaderResourceDescriptorHeapStart;
	D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessDescriptorHeapStart;
};
