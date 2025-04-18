#include "pch.h"
#include "Camera.h"

#include "Engine.h"
#include "GameObject.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "ParticleSystemComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Shader.h"
#include "Transform.h"

FMatrix Camera::StaticViewMatrix;
FMatrix Camera::StaticProjectionMatrix;

Camera::Camera()
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

Camera::~Camera() = default;

void Camera::FinalUpdate()
{
	ViewMatrix = GetTransform()->GetLocalToWorldMatrix().Invert();	// 월드 행렬의 역행렬

	float Width = static_cast<float>(GEngine->GetWindow().Width);
	float Height = static_cast<float>(GEngine->GetWindow().Height);

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

void Camera::SortGameObject()
{
	auto ActiveScene = SceneManager::Get()->GetActiveScene();
	auto& GameObjects = ActiveScene->GetGameObjects();

	DeferredShaders.clear();
	ForwardShaders.clear();
	ParticleShaders.clear();

	for (auto& GameObject : GameObjects)
	{
		auto MeshRenderer = GameObject->GetMeshRenderer();
		auto ParticleSystemComponent = GameObject->GetParticleSystemComponent();
		if (MeshRenderer == nullptr && ParticleSystemComponent == nullptr)
		{
			continue;
		}

		if ( IsLayerCulled(GameObject->GetLayer()) )
		{
			continue;
		}

		if (GameObject->GetCheckFrustum())
		{
			auto Transform = GameObject->GetTransform();
			if (!Frustum.ContainsSphere(Transform->GetWorldPosition(), Transform->GetFrustumBound()))
			{
				continue;
			}
		}

		if (MeshRenderer)
		{
			switch (MeshRenderer->GetMaterial()->GetShader()->GetShaderType())
			{
			case EShaderType::Deferred:
				DeferredShaders.push_back(GameObject);
				break;
			case EShaderType::Forward:
				ForwardShaders.push_back(GameObject);
				break;
			}
		}
		else if (ParticleSystemComponent)
		{
			ParticleShaders.push_back(GameObject);
		}
	}
}

void Camera::RenderDeferred()
{
	StaticViewMatrix = ViewMatrix;
	StaticProjectionMatrix = ProjectionMatrix;

	for (auto& GameObject : DeferredShaders)
	{
		if (auto MeshRenderer = GameObject->GetMeshRenderer())
		{
			MeshRenderer->Render();
		}
	}
}

void Camera::RenderForward()
{
	StaticViewMatrix = ViewMatrix;
	StaticProjectionMatrix = ProjectionMatrix;

	for (auto& GameObject : ForwardShaders)
	{
		if ( auto MeshRenderer = GameObject->GetMeshRenderer() )
		{
			MeshRenderer->Render();
		}
	}

	for(auto& ParticleObject : ParticleShaders)
	{
		if (auto ParticleSystemComponent = ParticleObject->GetParticleSystemComponent() )
		{
			ParticleSystemComponent->Render();
		}
	}
}
