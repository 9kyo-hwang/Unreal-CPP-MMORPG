#include "pch.h"
#include "TableDescriptorHeap.h"

#include "Engine.h"

FGraphicsResourceTables::FGraphicsResourceTables()
	: IncrementSize(0)
	, Size(0)
	, Count(0)
	, Top(0)
{
}

FGraphicsResourceTables::~FGraphicsResourceTables() = default;

void FGraphicsResourceTables::Initialize(ComPtr<ID3D12Device> Device, uint32 GroupCount)
{
	Count = GroupCount;

	D3D12_DESCRIPTOR_HEAP_DESC Desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = GroupCount * (NumCBVSRVRegister - 1),	// 그룹 개수 * 각 그룹 별 View(Register) 개수, b0는 전역으로 사용하기 때문에 -1
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,	// ShaderVisible로 해줘야 GPU DRAM에 상주, 사용 가능
	};
	assert(SUCCEEDED(Device->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&Heap))));

	IncrementSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Size = IncrementSize * ( NumCBVSRVRegister - 1 );
}

void FGraphicsResourceTables::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE Src, EConstantBufferViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE Dest = GetCPUHandle(Register);

	uint32 DestRange = 1;
	uint32 SrcRange = 1;

	// CPU쪽 ConstantBufferView DescriptorHeap -> GPU쪽 TableDescriptorHeap으로 복사
	DEVICE->CopyDescriptors(1, &Dest, &DestRange, 1, &Src, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FGraphicsResourceTables::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE Src, EShaderResourceViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE Dest = GetCPUHandle(Register);

	uint32 DestRange = 1;
	uint32 SrcRange = 1;
	DEVICE->CopyDescriptors(1, &Dest, &DestRange, 1, &Src, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

// Register에 있는 Table에 View DescriptorHeap 정보를 올려주는 용도
void FGraphicsResourceTables::Commit()
{
	D3D12_GPU_DESCRIPTOR_HANDLE Handle = Heap->GetGPUDescriptorHandleForHeapStart();
	Handle.ptr += Top * Size;
	GRAPHICS_COMMAND_LIST->SetGraphicsRootDescriptorTable(1, Handle);	// 현재 사용 중인 그룹을 올림

	Top++;
}

D3D12_CPU_DESCRIPTOR_HANDLE FGraphicsResourceTables::GetCPUHandle(EConstantBufferViewRegisters Register)
{
	return GetCPUHandle(StaticCast<uint8>(Register));
}

D3D12_CPU_DESCRIPTOR_HANDLE FGraphicsResourceTables::GetCPUHandle(EShaderResourceViewRegisters Register)
{
	return GetCPUHandle(StaticCast<uint8>(Register));
}

D3D12_CPU_DESCRIPTOR_HANDLE FGraphicsResourceTables::GetCPUHandle(uint8 Register)
{
	assert(Register > 0);	// b0는 이제 들어오지 않음

	D3D12_CPU_DESCRIPTOR_HANDLE Handle = Heap->GetCPUDescriptorHandleForHeapStart();
	Handle.ptr += Top * Size;	// 적절한 그룹 찾기
	Handle.ptr += (Register - 1) * IncrementSize;	// 해당 그룹 내 적절한 레지스터(View) 찾기
	return Handle;
}

FComputeResourceTables::FComputeResourceTables()
	: IncrementSize(0)
{
}

FComputeResourceTables::~FComputeResourceTables() = default;

void FComputeResourceTables::Initialize(ComPtr<ID3D12Device> Device)
{
	D3D12_DESCRIPTOR_HEAP_DESC Desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = NumTotalRegister,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	};
	assert(SUCCEEDED(Device->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&Heap))));
	IncrementSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FComputeResourceTables::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle,
	EConstantBufferViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE DestHandle = GetCPUHandle(Register);
	uint32 DestRange = 1, SrcRange = 1;
	DEVICE->CopyDescriptors(1, &DestHandle, &DestRange, 1, &SrcHandle, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FComputeResourceTables::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle,
	EShaderResourceViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE DestHandle = GetCPUHandle(Register);
	uint32 DestRange = 1, SrcRange = 1;
	DEVICE->CopyDescriptors(1, &DestHandle, &DestRange, 1, &SrcHandle, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FComputeResourceTables::SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle,
	EUnorderedAccessViewRegisters Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE DestHandle = GetCPUHandle(Register);
	uint32 DestRange = 1, SrcRange = 1;
	DEVICE->CopyDescriptors(1, &DestHandle, &DestRange, 1, &SrcHandle, &SrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FComputeResourceTables::Commit()
{
	ID3D12DescriptorHeap* DescriptorHeaps = Heap.Get();
	COMPUTE_COMMAND_LIST->SetDescriptorHeaps(1, &DescriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE Handle = DescriptorHeaps->GetGPUDescriptorHandleForHeapStart();
	COMPUTE_COMMAND_LIST->SetComputeRootDescriptorTable(0, Handle);
}

D3D12_CPU_DESCRIPTOR_HANDLE FComputeResourceTables::GetCPUHandle(EConstantBufferViewRegisters Register)
{
	return GetCPUHandle(StaticCast< uint8 >( Register ));
}

D3D12_CPU_DESCRIPTOR_HANDLE FComputeResourceTables::GetCPUHandle(EShaderResourceViewRegisters Register)
{
	return GetCPUHandle(StaticCast< uint8 >( Register ));
}

D3D12_CPU_DESCRIPTOR_HANDLE FComputeResourceTables::GetCPUHandle(EUnorderedAccessViewRegisters Register)
{
	return GetCPUHandle(StaticCast< uint8 >( Register ));
}

D3D12_CPU_DESCRIPTOR_HANDLE FComputeResourceTables::GetCPUHandle(uint8 Register)
{
	D3D12_CPU_DESCRIPTOR_HANDLE HeapStart = Heap->GetCPUDescriptorHandleForHeapStart();
	HeapStart.ptr += Register * IncrementSize;
	return HeapStart;
}
