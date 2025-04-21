#pragma once

class AActor;
struct FInstanceParameters;
class FInstanceBuffer;

class UInstanceManager
{
    GENERATED_SINGLETON(UInstanceManager);
    
public:
    void Render(vector<TSharedPtr<AActor>>& InActors);
    void ClearBuffer();
    
    void Empty() { BufferMap.clear(); }

private:
    // Vertex/Index Buffer랑 Shader 정보 모두가 일치하는 경우 동일한 인스턴스 
    void AddInstance(uint64 InstanceID, FInstanceParameters& Parameters);

    unordered_map<uint64/*Instance ID*/, TSharedPtr<FInstanceBuffer>> BufferMap;
};
