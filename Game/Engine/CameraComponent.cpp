#include "pch.h"
#include "CameraComponent.h"

#include "Engine.h"
#include "Actor.h"
#include "InstanceManager.h"
#include "Material.h"
#include "MeshComponent.h"
#include "ParticleSystemComponent.h"
#include "Level.h"
#include "SceneManager.h"
#include "Shader.h"
#include "SceneComponent.h"

FMatrix UCameraComponent::StaticViewMatrix;
FMatrix UCameraComponent::StaticProjectionMatrix;

UCameraComponent::UCameraComponent()
	: Super(EComponentType::Camera)
	, Projection(ECameraProjectionType::Perspective)
	, Near(1.f)
	, Far(1000.f)
	, Fov(XM_PI / 4.f) // 45도
	, Scale(1.f)
	, Width(StaticCast<float>(GEngine->GetWindow().Width))
	, Height(StaticCast<float>(GEngine->GetWindow().Height))
	, ViewMatrix()
	, ProjectionMatrix()
	, CullingMask(0)
{
}

UCameraComponent::~UCameraComponent() = default;

void UCameraComponent::FinalUpdate(float DeltaTime)
{
	ViewMatrix = GetTransform()->GetLocalToWorldMatrix().Invert();	// 월드 행렬의 역행렬

	switch (Projection)
	{
	case ECameraProjectionType::Perspective:
		ProjectionMatrix = XMMatrixPerspectiveFovLH(Fov, Width / Height, Near, Far);
		break;
	case ECameraProjectionType::Orthographic:
		ProjectionMatrix = XMMatrixOrthographicLH(Width * Scale, Height * Scale, Near, Far);
		break;
	}

	Frustum.FinalUpdate();
}

void UCameraComponent::SortActors()
{
	TSharedPtr<ULevel> CurrentLevel = SceneManager::Get()->GetCurrentLevel();
	auto& Actors = CurrentLevel->GetAllLevelActors();

	DeferredShaders.clear();
	ForwardShaders.clear();
	ParticleShaders.clear();

	for (auto& Actor : Actors)
	{
		if ( IsLayerCulled(Actor->GetLayer()) )
		{
			continue;
		}

		if (Actor->GetCheckFrustum())
		{
			auto Transform = Actor->GetSceneComponent();
			if (!Frustum.ContainsSphere(Transform->GetWorldPosition(), Transform->GetFrustumBound()))
			{
				continue;
			}
		}

		if (TSharedPtr<UMeshComponent> MeshComponent = Actor->GetMeshComponent())
		{
			switch (MeshComponent->GetMaterial()->GetShader()->GetShaderType())
			{
			case EShaderType::Deferred:
				DeferredShaders.push_back(Actor);
				break;
			case EShaderType::Forward:
				ForwardShaders.push_back(Actor);
				break;
			}
		}
		else if (TSharedPtr<UParticleSystemComponent> ParticleSystemComponent = Actor->GetParticleSystemComponent())
		{
			ParticleShaders.push_back(Actor);
		}
	}
}

void UCameraComponent::SortShadowActors()
{
	ShadowShaders.clear();

	TSharedPtr<ULevel> CurrentLevel = SceneManager::Get()->GetCurrentLevel();
	for (auto& Actor : CurrentLevel->GetAllLevelActors())
	{
		if (!Actor->GetMeshComponent())
		{
			continue;
		}

		if (Actor->IsStaticShadow())
		{
			continue;
		}

		if (IsLayerCulled(Actor->GetLayer()))
		{
			continue;
		}

		if ( Actor->GetCheckFrustum() )
		{
			auto SceneComponent = Actor->GetSceneComponent();
			if (!Frustum.ContainsSphere(SceneComponent->GetWorldPosition(), SceneComponent->GetFrustumBound()) )
			{
				continue;
			}
		}

		ShadowShaders.push_back(Actor);
	}
}

void UCameraComponent::RenderShadow()
{
	StaticViewMatrix = ViewMatrix;
	StaticProjectionMatrix = ProjectionMatrix;

	for (auto& Actor : ShadowShaders)
	{
		Actor->GetMeshComponent()->RenderShadow();
	}
}

void UCameraComponent::RenderDeferred()
{
	StaticViewMatrix = ViewMatrix;
	StaticProjectionMatrix = ProjectionMatrix;

	UInstanceManager::Get()->Render(DeferredShaders);
}

void UCameraComponent::RenderForward()
{
	StaticViewMatrix = ViewMatrix;
	StaticProjectionMatrix = ProjectionMatrix;

	UInstanceManager::Get()->Render(ForwardShaders);

	for(auto& ParticleObject : ParticleShaders)
	{
		if (auto ParticleSystemComponent = ParticleObject->GetParticleSystemComponent() )
		{
			ParticleSystemComponent->Render();
		}
	}
}
