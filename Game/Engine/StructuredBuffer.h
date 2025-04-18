#pragma once

class FStructuredBuffer
{
public:
    FStructuredBuffer();
    ~FStructuredBuffer();

    void Initialize(uint32 ElementSize, uint32 ElementCount);

    void PushGraphicsData(EShaderResourceViewRegisters Register);
    void PushComputeData(EShaderResourceViewRegisters Register);
    void PushComputeData(EUnorderedAccessViewRegisters Register);

    ComPtr<ID3D12DescriptorHeap> GetShaderResourceDescriptorHeap() { return ShaderResourceDescriptorHeap; }
    ComPtr<ID3D12DescriptorHeap> GetUnorderedAccessDescriptorHeap() { return UnorderedAccessDescriptorHeap; }

    D3D12_RESOURCE_STATES GetResourceState() const { return ResourceState; }
    void SetResourceState(D3D12_RESOURCE_STATES InResourceState) { ResourceState = InResourceState; }
    ComPtr<ID3D12Resource> GetData() { return Data; }

private:
    void CreateStructuredBuffer();
    void CreateShaderResourceDescriptor();
    void CreateUnorderedAccessDescriptor();
    
    ComPtr<ID3D12Resource> Data;
    ComPtr<ID3D12DescriptorHeap> ShaderResourceDescriptorHeap;
    ComPtr<ID3D12DescriptorHeap> UnorderedAccessDescriptorHeap;

    uint32 Size;
    uint32 Count;
    D3D12_RESOURCE_STATES ResourceState;

    D3D12_CPU_DESCRIPTOR_HANDLE ShaderResourceDescriptorHeapStart;
    D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessDescriptorHeapStart;
};
