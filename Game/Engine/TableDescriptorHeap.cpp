#include "pch.h"
#include "TableDescriptorHeap.h"

#include "Engine.h"

void FTableDescriptorHeap::Initialize(uint32 Count)
{
	GroupCount = Count;

	D3D12_DESCRIPTOR_HEAP_DESC Desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = Count * REGISTER_COUNT,	// 그룹 개수 * 각 그룹 별 View(Register) 개수
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,	// ShaderVisible로 해줘야 GPU DRAM에 상주, 사용 가능
	};

	DEVICE->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&Data));

	ViewSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	GroupSize = ViewSize * REGISTER_COUNT;
}

void FTableDescriptorHeap::Clear()
{
	CurrentGroupIndex = 0;
}

void FTableDescriptorHeap::SetConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE Src, EConstantBufferViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE Dest = GetCPUHandle(Register);

	uint32 DestRange = 1;
	uint32 SrcRange = 1;

	// CPU쪽 ConstantBufferView DescriptorHeap -> GPU쪽 TableDescriptorHeap으로 복사
	DEVICE->CopyDescriptors(1, &Dest, &DestRange, 1, &Src, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FTableDescriptorHeap::SetShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE Src, EShaderResourceViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE Dest = GetCPUHandle(Register);

	uint32 DestRange = 1;
	uint32 SrcRange = 1;
	DEVICE->CopyDescriptors(1, &Dest, &DestRange, 1, &Src, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// Register에 있는 Table에 View DescriptorHeap 정보를 올려주는 용도
void FTableDescriptorHeap::CommitTable()
{
	D3D12_GPU_DESCRIPTOR_HANDLE Handle = Data->GetGPUDescriptorHandleForHeapStart();
	Handle.ptr += CurrentGroupIndex * GroupSize;
	COMMAND_LIST->SetGraphicsRootDescriptorTable(0, Handle);	// 현재 사용 중인 그룹을 올림

	CurrentGroupIndex++;
}

D3D12_CPU_DESCRIPTOR_HANDLE FTableDescriptorHeap::GetCPUHandle(EConstantBufferViewRegisters Register)
{
	return GetCPUHandle(static_cast<uint8>(Register));
}

D3D12_CPU_DESCRIPTOR_HANDLE FTableDescriptorHeap::GetCPUHandle(EShaderResourceViewRegisters Register)
{
	return GetCPUHandle(static_cast<uint8>(Register));
}

D3D12_CPU_DESCRIPTOR_HANDLE FTableDescriptorHeap::GetCPUHandle(uint8 RegisterNumber)
{
	D3D12_CPU_DESCRIPTOR_HANDLE Handle = Data->GetCPUDescriptorHandleForHeapStart();
	Handle.ptr += CurrentGroupIndex * GroupSize;	// 적절한 그룹 찾기
	Handle.ptr += RegisterNumber * ViewSize;		// 해당 그룹 내 적절한 레지스터(View) 찾기
	return Handle;
}
