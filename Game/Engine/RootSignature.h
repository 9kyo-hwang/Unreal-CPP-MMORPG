#pragma once

class FRootSignature
{
public:
	void Initialize();
	ComPtr<ID3D12RootSignature> GetD3DRootSignature() { return RootSignature; }

private:
	ComPtr<ID3D12RootSignature> RootSignature;
};
