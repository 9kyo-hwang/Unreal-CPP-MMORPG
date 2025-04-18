#include "pch.h"
#include "ConstantBuffer.h"

#include "Engine.h"

FConstantBuffer::FConstantBuffer()
	: MappedElement(nullptr)
	, Size(0)
	, Count(0)
	, Top(0)
	, DescriptorHandle()
	, IncrementSize(0)
	, Register()
{
}

FConstantBuffer::~FConstantBuffer()
{
	if (Data)
	{
		Data->Unmap(0, nullptr);
		Data = nullptr;
	}
}

void FConstantBuffer::Initialize(EConstantBufferViewRegisters InRegister, uint32 ElementSize, uint32 ElementCount)
{
	Register = InRegister;
	
	/**
	 *	상수 버퍼는 256 바이트의 배수로 만들어야 함
	 *	~255: 1111 1111 .... 0000 0000(하위 8비트만 0)
	 *	어떤 수든 하위 8비트를 꺼버리는 역할, 즉 256의 배수로 만들어주는 기능 수행
	 *	그런데 이러면 내림 연산이 동작하여, 데이터보다 크기가 작은 버퍼가 할당됨
	 *	이를 해결하고자 255를 더해줘 그 숫자보다 크거나 같은, 가장 작은 256의 배수를 반환해줌
	 *	256이 아닌 255를 더하는 이유는, 원래 주어진 숫자가 256의 배수이면 그 자체를 그대로 쓰기 위함
	 */

	Size = (ElementSize + 255) & ~255;
	Count = ElementCount;

	CreateDescriptorHeap();
	CreateDescriptor();
}

void FConstantBuffer::PushGraphicsData(void* NewData, uint32 NewDataSize)
{
	assert(Top < Count);
	assert(Size == (NewDataSize + 255 & ~255));

	::memcpy(&MappedElement[Top * Size], NewData, NewDataSize);

	// TableDescriptorHeap 추가로 RootDescriptor가 삭제됨 -> 아래 코드 크래시!
	// D3D12_GPU_VIRTUAL_ADDRESS Address = GetGPUVirtualAddress(CurrentIndex);
	// GRAPHICS_COMMAND_LIST->SetGraphicsRootConstantBufferView(RootParameterIndex, Address);

	// 이제 추가한 데이터의 주소를 TableDescriptorHeap에게 넘겨줘야 함
	// 기존 Mesh에서 수행하던 작업을 내부에서 수행하도록 변경(레지스터 종류가 늘어남에 따라 잘못된 레지스터를 매핑하는 실수를 막기 위해)
	GEngine->GetGraphicsDescriptorTable()->SetDescriptor(GetCPUHandle(Top), Register);
	Top++;
}

void FConstantBuffer::PushComputeData(void* NewData, uint32 NewDataSize)
{
	assert(Top < Count);
	assert(Size == ( NewDataSize + 255 & ~255 ));

	::memcpy(&MappedElement[Top * Size], NewData, NewDataSize);

	GEngine->GetComputeDescriptorTable()->SetDescriptor(GetCPUHandle(Top), Register);
	Top++;
}

void FConstantBuffer::SetGlobalGraphicsData(void* InData, uint32 InDataSize)
{
	assert(Size == (( InDataSize + 255 ) & ~255));
	::memcpy(&MappedElement[0], InData, InDataSize);
	GRAPHICS_COMMAND_LIST->SetGraphicsRootConstantBufferView(0, GetGPUVirtualAddress(0));
}

D3D12_GPU_VIRTUAL_ADDRESS FConstantBuffer::GetGPUVirtualAddress(uint32 Index)
{
	D3D12_GPU_VIRTUAL_ADDRESS BeginAddress = Data->GetGPUVirtualAddress();
	BeginAddress += Index * Size;
	return BeginAddress;
}

D3D12_CPU_DESCRIPTOR_HANDLE FConstantBuffer::GetCPUHandle(uint32 Index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(DescriptorHandle, Index * IncrementSize);
}

void FConstantBuffer::CreateDescriptorHeap()
{
	uint32 HeapAllocSize = Size * Count;
	D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(HeapAllocSize);

	DEVICE->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&Desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&Data)
	);

	Data->Map(0, nullptr, reinterpret_cast<void**>(&MappedElement));
	// 여기서는 Unmap을 할 필요가 없음
}

void FConstantBuffer::CreateDescriptor()
{
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = Count,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,	// 효율적으로 동작하기 위해 ShaderVisible X
	};
	DEVICE->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescriptorHeap));

	DescriptorHandle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	IncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// CBV DescriptorHeap에 들어갈 각 원소(CBV) 생성
	for (uint32 Index = 0; Index < Count; ++Index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentDescriptor = GetCPUHandle(Index);
		D3D12_CONSTANT_BUFFER_VIEW_DESC DescriptorDesc
		{
			// DescriptorHeap 내부 각각의 CBV들이 Constant Buffer의 어떤 원소를 가리켜야 하는가
			.BufferLocation = Data->GetGPUVirtualAddress() + static_cast<uint64>(Size) * Index,
			.SizeInBytes = Size,
		};

		DEVICE->CreateConstantBufferView(&DescriptorDesc, CurrentDescriptor);
	}
}
