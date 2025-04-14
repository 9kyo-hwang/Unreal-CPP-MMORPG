#include "pch.h"
#include "Camera.h"

#include "Engine.h"
#include "GameObject.h"
#include "Material.h"
#include "MeshRenderer.h"
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
	, Fov(XM_PI / 4.f)	// 45도
	, Scale(1.f)
	, ViewMatrix()
	, ProjectionMatrix()
{
}

Camera::~Camera()
{
}

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
	const auto& GameObjects = ActiveScene->GetGameObjects();

	Shaders[0].clear(); Shaders[1].clear();

	for (auto& GameObject : GameObjects)
	{
		if (auto MeshRenderer = GameObject->GetMeshRenderer())
		{
			if (IsLayerCulled(GameObject->GetLayer()))
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

			EShaderType ShaderType = MeshRenderer->GetMaterial()->GetShader()->GetShaderType();
			Shaders[static_cast<uint8>(ShaderType)].emplace_back(GameObject);
		}
	}
}

void Camera::RenderDeferred()
{
	StaticViewMatrix = ViewMatrix;
	StaticProjectionMatrix = ProjectionMatrix;

	for (auto& GameObject : Shaders[static_cast<uint8>(EShaderType::Deferred)])
	{
		GameObject->GetMeshRenderer()->Render();
	}
}

void Camera::RenderForward()
{
	StaticViewMatrix = ViewMatrix;
	StaticProjectionMatrix = ProjectionMatrix;

	for (auto& GameObject : Shaders[static_cast<uint8>(EShaderType::Forward)])
	{
		GameObject->GetMeshRenderer()->Render();
	}
}
