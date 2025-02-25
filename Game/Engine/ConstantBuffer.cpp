#include "pch.h"
#include "ConstantBuffer.h"

#include "Engine.h"

FConstantBuffer::FConstantBuffer()
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

void FConstantBuffer::Initialize(uint32 Size, uint32 Count)
{
	/**
	 *	상수 버퍼는 256 바이트의 배수로 만들어야 함
	 *	~255: 1111 1111 .... 0000 0000(하위 8비트만 0)
	 *	어떤 수든 하위 8비트를 꺼버리는 역할, 즉 256의 배수로 만들어주는 기능 수행
	 *	그런데 이러면 내림 연산이 동작하여, 데이터보다 크기가 작은 버퍼가 할당됨
	 *	이를 해결하고자 255를 더해줘 그 숫자보다 크거나 같은, 가장 작은 256의 배수를 반환해줌
	 *	256이 아닌 255를 더하는 이유는, 원래 주어진 숫자가 256의 배수이면 그 자체를 그대로 쓰기 위함
	 */

	ElementSize = (Size + 255) & ~255;
	ElementCount = Count;
	CreateBuffer();
}

void FConstantBuffer::Add(int32 RootParameterIndex, void* InData, uint32 InDataSize)
{
	assert(CurrentIndex < ElementSize);

	::memcpy(&MappedElement[CurrentIndex * ElementSize], InData, InDataSize);

	D3D12_GPU_VIRTUAL_ADDRESS Address = GetGPUVirtualAddress(CurrentIndex);
	COMMAND_LIST->SetGraphicsRootConstantBufferView(RootParameterIndex, Address);
	CurrentIndex++;
}

D3D12_GPU_VIRTUAL_ADDRESS FConstantBuffer::GetGPUVirtualAddress(uint32 Index)
{
	D3D12_GPU_VIRTUAL_ADDRESS BeginAddress = Data->GetGPUVirtualAddress();
	BeginAddress += Index * ElementSize;
	return BeginAddress;
}

void FConstantBuffer::CreateBuffer()
{
	uint32 BufferSize = ElementSize * ElementCount;
	D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

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
