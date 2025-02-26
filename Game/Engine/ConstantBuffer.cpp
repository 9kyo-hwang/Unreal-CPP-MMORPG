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
	CreateView();
}

D3D12_CPU_DESCRIPTOR_HANDLE FConstantBuffer::Add(int32 RootParameterIndex, void* InData, uint32 InDataSize)
{
	assert(CurrentIndex < ElementSize);

	::memcpy(&MappedElement[CurrentIndex * ElementSize], InData, InDataSize);

	// TableDescriptorHeap 추가로 RootDescriptor가 삭제됨 -> 아래 코드 크래시!
	// D3D12_GPU_VIRTUAL_ADDRESS Address = GetGPUVirtualAddress(CurrentIndex);
	// COMMAND_LIST->SetGraphicsRootConstantBufferView(RootParameterIndex, Address);

	// 이제 추가한 데이터의 주소를 TableDescriptorHeap에게 넘겨줘야 함
	D3D12_CPU_DESCRIPTOR_HANDLE Handle = GetCPUHandle(CurrentIndex);
	CurrentIndex++;
	return Handle;
}

D3D12_GPU_VIRTUAL_ADDRESS FConstantBuffer::GetGPUVirtualAddress(uint32 Index)
{
	D3D12_GPU_VIRTUAL_ADDRESS BeginAddress = Data->GetGPUVirtualAddress();
	BeginAddress += Index * ElementSize;
	return BeginAddress;
}

D3D12_CPU_DESCRIPTOR_HANDLE FConstantBuffer::GetCPUHandle(uint32 Index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(ListBegin, Index * ListOffset);
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

void FConstantBuffer::CreateView()
{
	D3D12_DESCRIPTOR_HEAP_DESC ConstantBufferViewListDesc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = ElementCount,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,	// 효율적으로 동작하기 위해 ShaderVisible X
	};

	DEVICE->CreateDescriptorHeap(&ConstantBufferViewListDesc, IID_PPV_ARGS(&ConstantBufferViewList));

	ListBegin = ConstantBufferViewList->GetCPUDescriptorHandleForHeapStart();
	ListOffset = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// CBV DescriptorHeap에 들어갈 각 원소(CBV) 생성
	for (uint32 Index = 0; Index < ElementCount; ++Index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentViewLocation = GetCPUHandle(Index);
		D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc
		{
			// DescriptorHeap 내부 각각의 CBV들이 Constant Buffer의 어떤 원소를 가리켜야 하는가
			.BufferLocation = Data->GetGPUVirtualAddress() + static_cast<uint64>(ElementSize) * Index,
			.SizeInBytes = ElementSize,
		};

		DEVICE->CreateConstantBufferView(&ConstantBufferViewDesc, CurrentViewLocation);
	}
}
