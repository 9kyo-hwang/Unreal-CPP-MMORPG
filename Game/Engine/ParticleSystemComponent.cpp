#include "pch.h"
#include "ParticleSystemComponent.h"

#include "Resources.h"
#include "StructuredBuffer.h"
#include "TimeManager.h"
#include "SceneComponent.h"

UParticleSystemComponent::UParticleSystemComponent()
    : Super(EComponentType::ParticleSystem)
    , MaxParticle(1000)
    , SpawnInterval(0.005f)
    , AccumulateTime(0.f)
    , MinTime(0.5f)
    , MaxTime(1.f)
    , MinSpeed(100)
    , MaxSpeed(50)
    , StartScale(10.f)
    , EndScale(5.f)
{
    ParticleBuffer = MakeShared<FStructuredBuffer>();
    ParticleBuffer->Initialize(sizeof(FParticleData), MaxParticle);

    SharedComputeBuffer = MakeShared<FStructuredBuffer>();
    SharedComputeBuffer->Initialize(sizeof(FSharedComputeData), 1);

    Mesh = Resources::Get()->LoadPoint();
    Material = Resources::Get()->Get<FMaterial>(L"Particle");

    TSharedPtr<FTexture> Texture = Resources::Get()->Load<FTexture>(
        L"Bubbles",
        L"..\\Resources\\Texture\\Particle\\Bubble.png"
        );

    Material->SetTexture(0, Texture);
    ComputeMaterial = Resources::Get()->Get<FMaterial>(L"ComputeParticle");
}

UParticleSystemComponent::~UParticleSystemComponent() = default;

void UParticleSystemComponent::FinalUpdate(float DeltaTime)
{
    AccumulateTime += DeltaTime;

    int32 bSpawn = 0;
    if(SpawnInterval < AccumulateTime)
    {
        AccumulateTime -= SpawnInterval;
        bSpawn = 1;
    }

    ParticleBuffer->PushComputeData(EUnorderedAccessViewRegisters::u0);
    SharedComputeBuffer->PushComputeData(EUnorderedAccessViewRegisters::u1);

    ComputeMaterial->SetParameter(0, MaxParticle);
    ComputeMaterial->SetParameter(1, bSpawn);
    ComputeMaterial->SetParameter(1, FVector2(DeltaTime, AccumulateTime));
    ComputeMaterial->SetParameter(0, FVector4(MinTime, MaxTime, MinSpeed, MaxSpeed));

    ComputeMaterial->Dispatch(1, 1, 1);
}

void UParticleSystemComponent::Render()
{
    GetTransform()->PushData();

    ParticleBuffer->PushGraphicsData(EShaderResourceViewRegisters::t9);
    Material->SetParameter(0, StartScale);
    Material->SetParameter(1, EndScale);
    Material->PushGraphicsData();

    Mesh->Render(MaxParticle);
}
