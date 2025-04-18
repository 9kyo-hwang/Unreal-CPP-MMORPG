#pragma once
#include "Component.h"

class FMesh;
class FMaterial;
class FStructuredBuffer;

struct FParticleData
{
    FVector3 WorldPosition;
    float Time;
    FVector3 WorldDirection;
    float ElapsedTime;
    int32 bAlive = 0;
    int32 Padding[3];
};

struct FSharedComputeData
{
    int32 NumSpawn;
    int32 Padding[3];
};

class UParticleSystemComponent : public Component
{
    using Super = Component;

public:
    UParticleSystemComponent();
    ~UParticleSystemComponent() override;

    void FinalUpdate() override;
    void Render();

    void Load(const wstring& Path) override {}
    void Save(const wstring& Path) override {}

private:
    shared_ptr<FStructuredBuffer> ParticleBuffer;
    shared_ptr<FStructuredBuffer> SharedComputeBuffer;
    int32 MaxParticle;

    shared_ptr<FMaterial> Material;
    shared_ptr<FMaterial> ComputeMaterial;
    shared_ptr<FMesh> Mesh;

    float SpawnInterval;
    float AccumulateTime;
    float MinTime;
    float MaxTime;
    float MinSpeed;
    float MaxSpeed;
    float StartScale;
    float EndScale;
};
