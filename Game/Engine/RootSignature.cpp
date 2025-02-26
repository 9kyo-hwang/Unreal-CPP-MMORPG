#include "pch.h"
#include "RootSignature.h"

#include "Engine.h"

void FRootSignature::Initialize()
{
	CD3DX12_DESCRIPTOR_RANGE Ranges[ ] =
	{
		// baseShaderRegister(0) 번호부터 numDescriptors(5) 개수만큼 사용할 것
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT, 0),
	};

	CD3DX12_ROOT_PARAMETER Parameters[1];	// Descriptor Table 개수
	Parameters[0].InitAsDescriptorTable(_countof(Ranges), Ranges);

	D3D12_ROOT_SIGNATURE_DESC Desc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(Parameters), Parameters);
	Desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;	// 입력 조립기 단계

	ComPtr<ID3DBlob> Blob;
	ComPtr<ID3DBlob> ErrorBlob;
	::D3D12SerializeRootSignature(&Desc, D3D_ROOT_SIGNATURE_VERSION_1, &Blob, &ErrorBlob);
	DEVICE->CreateRootSignature(0, Blob->GetBufferPointer(), Blob->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
}
 