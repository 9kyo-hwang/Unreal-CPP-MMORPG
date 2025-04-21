#pragma once

struct FInstanceParameters
{
    FMatrix WorldMatrix;
    FMatrix WorldViewMatrix;
    FMatrix WorldViewProjectionMatrix;
};

class FInstanceBuffer
{
public:
    FInstanceBuffer();
    ~FInstanceBuffer();

    void Initialize(uint32 InstanceCount = 10);
    void Push();
    
    uint64 GetID() const { return ID; }
    void SetID(uint64 NewID) { ID = NewID; }
    ComPtr<ID3D12Resource> GetData() { return Buffer; }
    D3D12_VERTEX_BUFFER_VIEW GetView() { return View; }
    
    void Add(FInstanceParameters& NewInstance) { Instances.emplace_back(NewInstance); }
    void Empty() { Instances.clear(); }
    uint32 Num() const { return StaticCast<uint32>(Instances.size()); }

private:
    uint64 ID;
    ComPtr<ID3D12Resource> Buffer;
    D3D12_VERTEX_BUFFER_VIEW View;

    vector<FInstanceParameters> Instances;
    uint32 Capacity;
};
