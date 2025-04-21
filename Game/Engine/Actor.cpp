#include "pch.h"
#include "Actor.h"

#include "CameraComponent.h"
#include "LightComponent.h"
#include "MeshComponent.h"
#include "MonoBehaviour.h"
#include "ParticleSystemComponent.h"
#include "SceneComponent.h"

AActor::AActor()
	: Super(EObjectType::Actor)
{
}

AActor::~AActor() = default;

void AActor::Initialize()
{
	for (TSharedPtr<UActorComponent>& Component : Components)
	{
		if (Component)
		{
			Component->InitializeComponent();
		}
	}

	for (TSharedPtr<MonoBehaviour>& Script : Scripts)
	{
		Script->InitializeComponent();
	}
}

void AActor::BeginPlay()
{
	for (TSharedPtr<UActorComponent>& Component : Components)
	{
		if (Component)
		{
			Component->BeginPlay();
		}
	}

	for (TSharedPtr<MonoBehaviour>& Script : Scripts)
	{
		Script->BeginPlay();
	}
}

void AActor::Tick(float DeltaTime)
{
	for (TSharedPtr<UActorComponent>& Component : Components )
	{
		if ( Component )
		{
			Component->TickComponent(DeltaTime);
		}
	}

	for (TSharedPtr<MonoBehaviour>& Script : Scripts )
	{
		Script->TickComponent(DeltaTime);
	}
}

void AActor::LateUpdate(float DeltaTime)
{
	for (TSharedPtr<UActorComponent>& Component : Components)
	{
		if (Component)
		{
			Component->LateUpdate(DeltaTime);
		}
	}

	for (TSharedPtr<MonoBehaviour>& Script : Scripts)
	{
		Script->LateUpdate(DeltaTime);
	}
}

void AActor::FinalUpdate(float DeltaTime)
{
	for (TSharedPtr<UActorComponent>& Component : Components )
	{
		if (Component)
		{
			Component->FinalUpdate(DeltaTime);
		}
	}

	// MonoBehaviour를 상속받는 스크립트들은 FinalUpdate를 수행하지 않음
}

TSharedPtr<UActorComponent> AActor::GetComponent(EComponentType Type) const
{
	uint8 Index = StaticCast<uint8>(Type);
	assert(Index < ComponentCount);
	return Components[Index];
}

void AActor::AddComponent(TSharedPtr<UActorComponent> Component)
{
	Component->SetOwner(AsShared(this));

	uint8 Index = StaticCast<uint8>(Component->GetComponentType());
	if (Index < ComponentCount)
	{
		Components[Index] = Component;
	}
	else
	{
		Scripts.push_back(DynamicCastSharedPtr<MonoBehaviour>( Component ));
	}
}

TSharedPtr<USceneComponent> AActor::GetSceneComponent() const
{
	return StaticCastSharedPtr<USceneComponent>( GetComponent(EComponentType::Scene) );
}

TSharedPtr<UMeshComponent> AActor::GetMeshComponent() const
{
	return StaticCastSharedPtr<UMeshComponent>( GetComponent(EComponentType::Mesh) );
}

TSharedPtr<UCameraComponent> AActor::GetCameraComponent() const
{
	return StaticCastSharedPtr<UCameraComponent>( GetComponent(EComponentType::Camera) );
}

TSharedPtr<ULightComponent> AActor::GetLightComponent() const
{
	return StaticCastSharedPtr<ULightComponent>( GetComponent(EComponentType::Light) );
}

TSharedPtr<UParticleSystemComponent> AActor::GetParticleSystemComponent() const
{
	return StaticCastSharedPtr<UParticleSystemComponent>(GetComponent(EComponentType::ParticleSystem));
}
