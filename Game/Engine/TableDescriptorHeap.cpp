#include "pch.h"
#include "TableDescriptorHeap.h"

#include "Engine.h"

void FGraphicsDescriptorTable::Initialize(uint32 Count)
{
	GroupCount = Count;

	D3D12_DESCRIPTOR_HEAP_DESC Desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = Count * (NumCBVSRVRegister - 1),	// 그룹 개수 * 각 그룹 별 View(Register) 개수, b0는 전역으로 사용하기 때문에 -1
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,	// ShaderVisible로 해줘야 GPU DRAM에 상주, 사용 가능
	};

	DEVICE->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&Data));

	ViewSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	GroupSize = ViewSize * ( NumCBVSRVRegister - 1 );
}

void FGraphicsDescriptorTable::Clear()
{
	CurrentGroupIndex = 0;
}

void FGraphicsDescriptorTable::SetConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE Src, EConstantBufferViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE Dest = GetCPUHandle(Register);

	uint32 DestRange = 1;
	uint32 SrcRange = 1;

	// CPU쪽 ConstantBufferView DescriptorHeap -> GPU쪽 TableDescriptorHeap으로 복사
	DEVICE->CopyDescriptors(1, &Dest, &DestRange, 1, &Src, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FGraphicsDescriptorTable::SetShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE Src, EShaderResourceViewRegisters Register)
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
	Handle.ptr += CurrentGroupIndex * GroupSize;
	GRAPHICS_COMMAND_LIST->SetGraphicsRootDescriptorTable(1, Handle);	// 현재 사용 중인 그룹을 올림

	CurrentGroupIndex++;
}

D3D12_CPU_DESCRIPTOR_HANDLE FGraphicsDescriptorTable::GetCPUHandle(EConstantBufferViewRegisters Register)
{
	return GetCPUHandle(static_cast<uint8>(Register));
}

D3D12_CPU_DESCRIPTOR_HANDLE FGraphicsDescriptorTable::GetCPUHandle(EShaderResourceViewRegisters Register)
{
	return GetCPUHandle(static_cast<uint8>(Register));
}

D3D12_CPU_DESCRIPTOR_HANDLE FGraphicsDescriptorTable::GetCPUHandle(uint8 RegisterNumber)
{
	assert(RegisterNumber > 0);	// b0는 이제 들어오지 않음

	D3D12_CPU_DESCRIPTOR_HANDLE Handle = Data->GetCPUDescriptorHandleForHeapStart();
	Handle.ptr += CurrentGroupIndex * GroupSize;	// 적절한 그룹 찾기
	Handle.ptr += (RegisterNumber - 1) * ViewSize;	// 해당 그룹 내 적절한 레지스터(View) 찾기
	return Handle;
}

FComputeDescriptorTable::FComputeDescriptorTable()
	: IncrementSize(0)
{
}

FComputeDescriptorTable::~FComputeDescriptorTable()
{
}

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
	ID3D12DescriptorHeap* DescriptorHeapPtr = DescriptorHeap.Get();
	COMPUTE_COMMAND_LIST->SetDescriptorHeaps(1, &DescriptorHeapPtr);

	D3D12_GPU_DESCRIPTOR_HANDLE Handle = DescriptorHeapPtr->GetGPUDescriptorHandleForHeapStart();
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
	D3D12_CPU_DESCRIPTOR_HANDLE Handle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	Handle.ptr += Register * IncrementSize;
	return Handle;
}
