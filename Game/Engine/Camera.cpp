#include "pch.h"
#include "Camera.h"

#include "Engine.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"

FMatrix Camera::StaticViewMatrix;
FMatrix Camera::StaticProjectionMatrix;

Camera::Camera()
	: Super(EComponentType::Camera)
	, Type(ECameraProjectionType::Perspective)
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

	switch (Type)
	{
	case ECameraProjectionType::Perspective:
		ProjectionMatrix = XMMatrixPerspectiveFovLH(Fov, Width / Height, Near, Far);
		break;
	case ECameraProjectionType::Orthographic:
		ProjectionMatrix = XMMatrixOrthographicLH(Width * Scale, Height * Scale, Near, Far);
		break;
	}

	StaticViewMatrix = ViewMatrix;
	StaticProjectionMatrix = ProjectionMatrix;

	Frustum.FinalUpdate();
}

void Camera::Render()
{
	// TODO: Layer 구분
	for (auto& GameObject : SceneManager::Get()->GetActiveScene()->GetGameObjects())
	{
		if (const auto& MeshRenderer = GameObject->GetMeshRenderer())
		{
			if (GameObject->GetCheckFrustum())
			{
				auto Transform = GameObject->GetTransform();
				if (!Frustum.ContainsSphere(Transform->GetWorldPosition(), Transform->GetFrustumBound()))
				{
					continue;
				}
			}

			MeshRenderer->Render();
		}
	}
}
