#pragma once
#include "ActorComponent.h"

class UMesh;
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

class UParticleSystemComponent : public UActorComponent
{
    using Super = UActorComponent;

public:
    UParticleSystemComponent();
    ~UParticleSystemComponent() override;

    void FinalUpdate(float DeltaTime) override;
    void Render();

    void Load(const wstring& Path) override {}
    void Save(const wstring& Path) override {}

private:
    TSharedPtr<FStructuredBuffer> ParticleBuffer;
    TSharedPtr<FStructuredBuffer> SharedComputeBuffer;
    int32 MaxParticle;

    TSharedPtr<FMaterial> Material;
    TSharedPtr<FMaterial> ComputeMaterial;
    TSharedPtr<UMesh> Mesh;

    float SpawnInterval;
    float AccumulateTime;
    float MinTime;
    float MaxTime;
    float MinSpeed;
    float MaxSpeed;
    float StartScale;
    float EndScale;
};
