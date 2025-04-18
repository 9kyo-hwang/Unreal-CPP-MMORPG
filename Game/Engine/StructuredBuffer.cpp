#include "pch.h"
#include "StructuredBuffer.h"

#include "Engine.h"

FStructuredBuffer::FStructuredBuffer()
    : Size(0)
    , Count(0)
    , ResourceState()
    , ShaderResourceDescriptorHeapStart()
    , UnorderedAccessDescriptorHeapStart()
{
}

FStructuredBuffer::~FStructuredBuffer() = default;

void FStructuredBuffer::Initialize(uint32 ElementSize, uint32 ElementCount)
{
    Size = ElementSize;
    Count = ElementCount;
    ResourceState = D3D12_RESOURCE_STATE_COMMON;

    CreateStructuredBuffer();
    CreateShaderResourceDescriptor();
    CreateUnorderedAccessDescriptor();
}

void FStructuredBuffer::PushGraphicsData(EShaderResourceViewRegisters Register)
{
    GEngine->GetGraphicsDescriptorTable()->SetDescriptor(ShaderResourceDescriptorHeapStart, Register);
}

void FStructuredBuffer::PushComputeData(EShaderResourceViewRegisters Register)
{
    GEngine->GetComputeDescriptorTable()->SetDescriptor(ShaderResourceDescriptorHeapStart, Register);
}

void FStructuredBuffer::PushComputeData(EUnorderedAccessViewRegisters Register)
{
    GEngine->GetComputeDescriptorTable()->SetDescriptor(UnorderedAccessDescriptorHeapStart, Register);
}

void FStructuredBuffer::CreateStructuredBuffer()
{
    uint64 BufferMallocSize = static_cast<uint64>(Size) * Count;
    D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(BufferMallocSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    assert(SUCCEEDED(DEVICE->CreateCommittedResource(
        &HeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &Desc,
        ResourceState,
        nullptr,
        IID_PPV_ARGS(&Data)
        )));
}

void FStructuredBuffer::CreateShaderResourceDescriptor()
{
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc
    {
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        1,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0
    };
    assert(SUCCEEDED(DEVICE->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&ShaderResourceDescriptorHeap))));
    ShaderResourceDescriptorHeapStart = ShaderResourceDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    D3D12_SHADER_RESOURCE_VIEW_DESC DescriptorDesc
    {
        .Format = DXGI_FORMAT_UNKNOWN,
        .ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
        .Buffer ={ 0, Count, Size, D3D12_BUFFER_SRV_FLAG_NONE }
    };
    DEVICE->CreateShaderResourceView(Data.Get(), &DescriptorDesc, ShaderResourceDescriptorHeapStart);
}

void FStructuredBuffer::CreateUnorderedAccessDescriptor()
{
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc
    {
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        1,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE
    };
    assert(SUCCEEDED(DEVICE->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&UnorderedAccessDescriptorHeap))));
    UnorderedAccessDescriptorHeapStart = UnorderedAccessDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    D3D12_UNORDERED_ACCESS_VIEW_DESC DescriptorDesc
    {
        .Format = DXGI_FORMAT_UNKNOWN,
        .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
        .Buffer = { 0, Count, Size, 0, D3D12_BUFFER_UAV_FLAG_NONE }
    };
    DEVICE->CreateUnorderedAccessView(Data.Get(), nullptr, &DescriptorDesc, UnorderedAccessDescriptorHeapStart);
}
