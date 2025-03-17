#include "pch.h"
#include "Transform.h"

#include "Camera.h"
#include "Engine.h"

Transform::Transform()
	: Super(EComponentType::Transform)
	, LocalScale(1.f, 1.f, 1.f)
{
}

Transform::~Transform()
{
}

void Transform::FinalUpdate()
{
	FMatrix Scale = FMatrix::CreateScale(LocalScale);

	// TODO: Quaternion 방식으로 변경해야 함
	FMatrix Rotation = FMatrix::CreateRotationX(LocalRotation.x);
	Rotation *= FMatrix::CreateRotationY(LocalRotation.y);
	Rotation *= FMatrix::CreateRotationZ(LocalRotation.z);

	FMatrix Translation = FMatrix::CreateTranslation(LocalPosition);

	LocalMatrix = Scale * Rotation * Translation;
	WorldMatrix = LocalMatrix;	// 부모가 없다면 이 자체가 월드 좌표

	if (false == GetParent().expired())
	{
		WorldMatrix *= GetParent().lock()->GetLocalToWorldMatrix();
	}
}

void Transform::PushData()
{
	FTransformParameters Parameters
	{
		WorldMatrix,
		Camera::StaticViewMatrix,
		Camera::StaticProjectionMatrix,
		WorldMatrix * Camera::StaticViewMatrix,
		WorldMatrix* Camera::StaticViewMatrix* Camera::StaticProjectionMatrix
	};
	
	// FConstantBuffer::PushData()에서 SetShaderResourceView()를 자동적으로 수행해주고 있음.
	CONSTANT_BUFFER(EConstantBufferType::Transform)->PushData(&Parameters, sizeof(Parameters));
}
