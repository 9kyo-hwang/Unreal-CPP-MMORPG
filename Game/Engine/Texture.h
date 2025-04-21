#pragma once
#include "Object.h"

class FTexture : public UObject
{
	using Super = UObject;

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
	ComPtr<ID3D12DescriptorHeap> GetSRVHeap() { return SRVHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRTVHeap() { return RTVHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDSVHeap() { return DSVHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAVHeap() { return UAVHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() { return SRVHeapStart; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAV() { return UAVHeapStart; }

private:
	ScratchImage Image;
	ComPtr<ID3D12Resource> Texture2D;

	// ConstantBuffer는 View를 여러 개 담은 Heap을 사용하지만, Texture는 Heap에 View 하나만 사용
	ComPtr<ID3D12DescriptorHeap> SRVHeap;
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;
	ComPtr<ID3D12DescriptorHeap> UAVHeap;

private:
	D3D12_CPU_DESCRIPTOR_HANDLE SRVHeapStart;
	D3D12_CPU_DESCRIPTOR_HANDLE UAVHeapStart;
};
