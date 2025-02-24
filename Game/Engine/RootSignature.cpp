#include "pch.h"
#include "RootSignature.h"

#include "Engine.h"

void FRootSignature::Initialize()
{
	D3D12_ROOT_SIGNATURE_DESC Desc = CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT);
	Desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// 입력 조립기 단계

	ComPtr<ID3DBlob> Blob;
	ComPtr<ID3DBlob> ErrorBlob;
	::D3D12SerializeRootSignature(&Desc, D3D_ROOT_SIGNATURE_VERSION_1, &Blob, &ErrorBlob);
	DEVICE->CreateRootSignature(0, Blob->GetBufferPointer(), Blob->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
}
