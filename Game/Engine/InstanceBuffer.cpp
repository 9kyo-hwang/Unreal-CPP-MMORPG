#include "pch.h"
#include "InstanceBuffer.h"

#include "Engine.h"

FInstanceBuffer::FInstanceBuffer()
    : ID(0)
    , View()
    , Capacity(0)
{
}

FInstanceBuffer::~FInstanceBuffer() = default;

void FInstanceBuffer::Initialize(uint32 InstanceCount)
{
    Capacity = InstanceCount;

    const int32 MallocSize = sizeof(FInstanceParameters) * Capacity;
    D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(MallocSize);

    assert(SUCCEEDED(DEVICE->CreateCommittedResource(
        &HeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &ResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&Buffer)
        )));
}

void FInstanceBuffer::Push()
{
    const uint32 CurrentInstanceCount = Num();
    if(CurrentInstanceCount > Capacity)
    {
        Initialize(CurrentInstanceCount);
    }

    const uint32 MallocSize = CurrentInstanceCount * sizeof(FInstanceParameters);
    void* Data = nullptr;
    D3D12_RANGE ReadRange{0, 0};
    
    Buffer->Map(0, &ReadRange, &Data);
    memcpy(Data, &Instances[0], MallocSize);
    Buffer->Unmap(0, nullptr);

    View.BufferLocation = Buffer->GetGPUVirtualAddress();
    View.StrideInBytes = sizeof(FInstanceParameters);
    View.SizeInBytes = MallocSize;
}
