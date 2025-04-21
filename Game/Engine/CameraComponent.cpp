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
	, ViewMatrix()
	, ProjectionMatrix()
	, CullingMask(0)
{
}

UCameraComponent::~UCameraComponent() = default;

void UCameraComponent::FinalUpdate(float DeltaTime)
{
	ViewMatrix = GetTransform()->GetLocalToWorldMatrix().Invert();	// 월드 행렬의 역행렬

	float Width = StaticCast<float>(GEngine->GetWindow().Width);
	float Height = StaticCast<float>(GEngine->GetWindow().Height);

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

void UCameraComponent::SortGameObject()
{
	auto ActiveScene = SceneManager::Get()->GetActiveScene();
	auto& GameObjects = ActiveScene->GetAllActor();

	DeferredShaders.clear();
	ForwardShaders.clear();
	ParticleShaders.clear();

	for (auto& GameObject : GameObjects)
	{
		if ( IsLayerCulled(GameObject->GetLayer()) )
		{
			continue;
		}

		if (GameObject->GetCheckFrustum())
		{
			auto Transform = GameObject->GetSceneComponent();
			if (!Frustum.ContainsSphere(Transform->GetWorldPosition(), Transform->GetFrustumBound()))
			{
				continue;
			}
		}

		if (TSharedPtr<UMeshComponent> MeshComponent = GameObject->GetMeshComponent())
		{
			switch (MeshComponent->GetMaterial()->GetShader()->GetShaderType())
			{
			case EShaderType::Deferred:
				DeferredShaders.push_back(GameObject);
				break;
			case EShaderType::Forward:
				ForwardShaders.push_back(GameObject);
				break;
			}
		}
		else if (TSharedPtr<UParticleSystemComponent> ParticleSystemComponent = GameObject->GetParticleSystemComponent())
		{
			ParticleShaders.push_back(GameObject);
		}
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
