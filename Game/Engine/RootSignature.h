#pragma once

class FRootSignature
{
public:
	void Initialize();
	ComPtr<ID3D12RootSignature> GetD3DRootSignature() { return RootSignature; }

private:
	void CreateSamplerDescription();
	void CreateRootSignature();

private:
	ComPtr<ID3D12RootSignature> RootSignature;
	D3D12_STATIC_SAMPLER_DESC SamplerDesc{};	// 추후 배열로 여러 개를 관리해야 함
};
