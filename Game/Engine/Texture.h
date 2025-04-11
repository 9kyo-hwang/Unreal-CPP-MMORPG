#pragma once
#include "Object.h"

class FTexture : public Object
{
	using Super = Object;

public:
	FTexture();
	~FTexture() override;

	void Load(const wstring& Path) override;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() { return ViewHandle; }

public:
	void CreateTexture(const wstring& Path);
	void CreateView();

private:
	ScratchImage Image;
	ComPtr<ID3D12Resource> Texture2D;

	// ConstantBuffer는 View를 여러 개 담은 Heap을 사용하지만, Texture는 Heap에 View 하나만 사용
	ComPtr<ID3D12DescriptorHeap> ShaderResourceViewList;
	D3D12_CPU_DESCRIPTOR_HANDLE ViewHandle{};
};
