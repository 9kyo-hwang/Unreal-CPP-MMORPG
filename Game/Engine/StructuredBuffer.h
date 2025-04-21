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

    ComPtr<ID3D12DescriptorHeap> GetSRVHeap() { return SRVHeap; }
    ComPtr<ID3D12DescriptorHeap> GetUAVHeap() { return UAVHeap; }

    D3D12_RESOURCE_STATES GetResourceState() const { return ResourceState; }
    void SetResourceState(D3D12_RESOURCE_STATES InResourceState) { ResourceState = InResourceState; }
    ComPtr<ID3D12Resource> GetData() { return Data; }

private:
    void CreateStructuredBuffer();
    void CreateSRV();
    void CreateUAV();
    
    ComPtr<ID3D12Resource> Data;
    ComPtr<ID3D12DescriptorHeap> SRVHeap;
    ComPtr<ID3D12DescriptorHeap> UAVHeap;

    uint32 Size;
    uint32 Count;
    D3D12_RESOURCE_STATES ResourceState;

    D3D12_CPU_DESCRIPTOR_HANDLE SRVHeapStart;
    D3D12_CPU_DESCRIPTOR_HANDLE UAVHeapStart;
};
