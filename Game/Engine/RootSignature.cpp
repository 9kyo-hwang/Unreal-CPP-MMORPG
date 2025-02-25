#include "pch.h"
#include "RootSignature.h"

#include "Engine.h"

void FRootSignature::Initialize()
{
	CD3DX12_ROOT_PARAMETER Parameters[2];
	Parameters[0].InitAsConstantBufferView(0);	// 0번 -> root CBV가 있음, b0로 활용
	Parameters[1].InitAsConstantBufferView(1);	// 1번 -> root CBV가 있음, b1로 활용

	D3D12_ROOT_SIGNATURE_DESC Desc = CD3DX12_ROOT_SIGNATURE_DESC(2, Parameters);
	Desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// 입력 조립기 단계

	ComPtr<ID3DBlob> Blob;
	ComPtr<ID3DBlob> ErrorBlob;
	::D3D12SerializeRootSignature(&Desc, D3D_ROOT_SIGNATURE_VERSION_1, &Blob, &ErrorBlob);
	DEVICE->CreateRootSignature(0, Blob->GetBufferPointer(), Blob->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
}
 