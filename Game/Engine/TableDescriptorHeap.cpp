#include "pch.h"
#include "TableDescriptorHeap.h"

#include "Engine.h"

FGraphicsDescriptorTable::FGraphicsDescriptorTable()
	: IncrementSize(0)
	, Size(0)
	, Count(0)
	, Top(0)
{
}

FGraphicsDescriptorTable::~FGraphicsDescriptorTable() = default;

void FGraphicsDescriptorTable::Initialize(uint32 GroupCount)
{
	Count = GroupCount;

	D3D12_DESCRIPTOR_HEAP_DESC Desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = GroupCount * (NumCBVSRVRegister - 1),	// 그룹 개수 * 각 그룹 별 View(Register) 개수, b0는 전역으로 사용하기 때문에 -1
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,	// ShaderVisible로 해줘야 GPU DRAM에 상주, 사용 가능
	};
	assert(SUCCEEDED(DEVICE->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&Data))));

	IncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Size = IncrementSize * ( NumCBVSRVRegister - 1 );
}

void FGraphicsDescriptorTable::Clear()
{
	Top = 0;
}

void FGraphicsDescriptorTable::SetDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE Src, EConstantBufferViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE Dest = GetCPUHandle(Register);

	uint32 DestRange = 1;
	uint32 SrcRange = 1;

	// CPU쪽 ConstantBufferView DescriptorHeap -> GPU쪽 TableDescriptorHeap으로 복사
	DEVICE->CopyDescriptors(1, &Dest, &DestRange, 1, &Src, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FGraphicsDescriptorTable::SetDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE Src, EShaderResourceViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE Dest = GetCPUHandle(Register);

	uint32 DestRange = 1;
	uint32 SrcRange = 1;
	DEVICE->CopyDescriptors(1, &Dest, &DestRange, 1, &Src, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// Register에 있는 Table에 View DescriptorHeap 정보를 올려주는 용도
void FGraphicsDescriptorTable::Commit()
{
	D3D12_GPU_DESCRIPTOR_HANDLE Handle = Data->GetGPUDescriptorHandleForHeapStart();
	Handle.ptr += Top * Size;
	GRAPHICS_COMMAND_LIST->SetGraphicsRootDescriptorTable(1, Handle);	// 현재 사용 중인 그룹을 올림

	Top++;
}

D3D12_CPU_DESCRIPTOR_HANDLE FGraphicsDescriptorTable::GetCPUHandle(EConstantBufferViewRegisters Register)
{
	return GetCPUHandle(static_cast<uint8>(Register));
}

D3D12_CPU_DESCRIPTOR_HANDLE FGraphicsDescriptorTable::GetCPUHandle(EShaderResourceViewRegisters Register)
{
	return GetCPUHandle(static_cast<uint8>(Register));
}

D3D12_CPU_DESCRIPTOR_HANDLE FGraphicsDescriptorTable::GetCPUHandle(uint8 Register)
{
	assert(Register > 0);	// b0는 이제 들어오지 않음

	D3D12_CPU_DESCRIPTOR_HANDLE Handle = Data->GetCPUDescriptorHandleForHeapStart();
	Handle.ptr += Top * Size;	// 적절한 그룹 찾기
	Handle.ptr += (Register - 1) * IncrementSize;	// 해당 그룹 내 적절한 레지스터(View) 찾기
	return Handle;
}

FComputeDescriptorTable::FComputeDescriptorTable()
	: IncrementSize(0)
{
}

FComputeDescriptorTable::~FComputeDescriptorTable()	= default;

void FComputeDescriptorTable::Initialize()
{
	D3D12_DESCRIPTOR_HEAP_DESC Desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = NumTotalRegister,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	};
	DEVICE->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&DescriptorHeap));

	IncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FComputeDescriptorTable::SetDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle,
	EConstantBufferViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE DestHandle = GetCPUHandle(Register);
	uint32 DestRange = 1, SrcRange = 1;
	DEVICE->CopyDescriptors(1, &DestHandle, &DestRange, 1, &SrcHandle, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FComputeDescriptorTable::SetDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle,
	EShaderResourceViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE DestHandle = GetCPUHandle(Register);
	uint32 DestRange = 1, SrcRange = 1;
	DEVICE->CopyDescriptors(1, &DestHandle, &DestRange, 1, &SrcHandle, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FComputeDescriptorTable::SetDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle,
	EUnorderedAccessViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE DestHandle = GetCPUHandle(Register);
	uint32 DestRange = 1, SrcRange = 1;
	DEVICE->CopyDescriptors(1, &DestHandle, &DestRange, 1, &SrcHandle, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FComputeDescriptorTable::Commit()
{
	ID3D12DescriptorHeap* DescriptorHeaps = DescriptorHeap.Get();
	COMPUTE_COMMAND_LIST->SetDescriptorHeaps(1, &DescriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE Handle = DescriptorHeaps->GetGPUDescriptorHandleForHeapStart();
	COMPUTE_COMMAND_LIST->SetComputeRootDescriptorTable(0, Handle);
}

D3D12_CPU_DESCRIPTOR_HANDLE FComputeDescriptorTable::GetCPUHandle(EConstantBufferViewRegisters Register)
{
	return GetCPUHandle(static_cast< uint8 >( Register ));
}

D3D12_CPU_DESCRIPTOR_HANDLE FComputeDescriptorTable::GetCPUHandle(EShaderResourceViewRegisters Register)
{
	return GetCPUHandle(static_cast< uint8 >( Register ));
}

D3D12_CPU_DESCRIPTOR_HANDLE FComputeDescriptorTable::GetCPUHandle(EUnorderedAccessViewRegisters Register)
{
	return GetCPUHandle(static_cast< uint8 >( Register ));
}

D3D12_CPU_DESCRIPTOR_HANDLE FComputeDescriptorTable::GetCPUHandle(uint8 Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE HeapStart = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	HeapStart.ptr += Register * IncrementSize;
	return HeapStart;
}
