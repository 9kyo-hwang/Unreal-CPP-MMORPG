#include "pch.h"
#include "SceneComponent.h"

#include "CameraComponent.h"
#include "Engine.h"

USceneComponent::USceneComponent()
	: Super(EComponentType::Scene)
	, LocalScale(1.f, 1.f, 1.f)
{
}

USceneComponent::~USceneComponent() = default;

void USceneComponent::FinalUpdate(float DeltaTime)
{
	FMatrix Scale = FMatrix::CreateScale(LocalScale);

	// TODO: Quaternion 방식으로 변경해야 함
	FMatrix Rotation = FMatrix::CreateRotationX(LocalRotation.x);
	Rotation *= FMatrix::CreateRotationY(LocalRotation.y);
	Rotation *= FMatrix::CreateRotationZ(LocalRotation.z);

	FMatrix Translation = FMatrix::CreateTranslation(LocalPosition);

	LocalMatrix = Scale * Rotation * Translation;
	WorldMatrix = LocalMatrix;	// 부모가 없다면 이 자체가 월드 좌표

	if (false == GetOwner().expired())
	{
		WorldMatrix *= GetOwner().lock()->GetLocalToWorldMatrix();
	}
}

void USceneComponent::PushData()
{
	FTransformParameters Parameters
	{
		WorldMatrix,
		UCameraComponent::StaticViewMatrix,
		UCameraComponent::StaticProjectionMatrix,
		WorldMatrix * UCameraComponent::StaticViewMatrix,
		WorldMatrix* UCameraComponent::StaticViewMatrix* UCameraComponent::StaticProjectionMatrix
	};
	
	// FConstantBuffer::PushGraphicsData()에서 SetShaderResourceView()를 자동적으로 수행해주고 있음.
	CONSTANT_BUFFER(EConstantBufferType::Transform)->PushGraphicsData(&Parameters, sizeof(Parameters));
}
