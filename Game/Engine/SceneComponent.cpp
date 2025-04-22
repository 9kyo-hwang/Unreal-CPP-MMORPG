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
		WorldMatrix * UCameraComponent::StaticViewMatrix * UCameraComponent::StaticProjectionMatrix,
		UCameraComponent::StaticViewMatrix.Invert()
	};
	
	// FConstantBuffer::PushGraphicsData()에서 SetShaderResourceView()를 자동적으로 수행해주고 있음.
	CONSTANT_BUFFER(EConstantBufferType::Transform)->PushGraphicsData(&Parameters, sizeof(Parameters));
}

void USceneComponent::LookAt(const FVector3& Direction)
{
	FVector3 Front = Direction;
	Front.Normalize();

	FVector3 Right = FVector3::Up.Cross(Direction);
	if (Right == FVector3::Zero)
	{
		Right = FVector3::Forward.Cross(Direction);
	}
	Right.Normalize();

	FVector3 Up = Front.Cross(Right);
	Up.Normalize();

	FMatrix Matrix = XMMatrixIdentity();
	Matrix.Right(Right);
	Matrix.Up(Up);
	Matrix.Backward(Front);	// SimpleMath에서 Matrix는 오른손 좌표계라 Forward <-> Backward 반대

	LocalRotation = DecomposeRotationMatrix(Matrix);
}

bool USceneComponent::CloseEnough(const float& Lhs, const float& Rhs, const float& Epsilon)
{
	return Epsilon > abs(Lhs - Rhs);
}

FVector3 USceneComponent::DecomposeRotationMatrix(const FMatrix& Rotation)
{
	FVector4 Vectors[4];
	XMStoreFloat4(&Vectors[0], Rotation.Right());
	XMStoreFloat4(&Vectors[1], Rotation.Up());
	XMStoreFloat4(&Vectors[2], Rotation.Backward());
	XMStoreFloat4(&Vectors[3], Rotation.Translation());

	FVector3 RetVal;
	if ( CloseEnough(Vectors[0].z, -1.0f) )
	{
		float x = 0;
		float y = XM_PI / 2;
		float z = x + atan2(Vectors[1].x, Vectors[2].x);
		RetVal = FVector3(x, y, z);
	}
	else if ( CloseEnough(Vectors[0].z, 1.0f) )
	{
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2(-Vectors[1].x, -Vectors[2].x);
		RetVal = FVector3(x, y, z);
	}
	else
	{
		float y1 = -asin(Vectors[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(Vectors[1].z / cos(y1), Vectors[2].z / cos(y1));
		float x2 = atan2f(Vectors[1].z / cos(y2), Vectors[2].z / cos(y2));

		float z1 = atan2f(Vectors[0].y / cos(y1), Vectors[0].x / cos(y1));
		float z2 = atan2f(Vectors[0].y / cos(y2), Vectors[0].x / cos(y2));

		if (abs(x1) + abs(y1) + abs(z1) <= abs(x2) + abs(y2) + abs(z2))
		{
			RetVal = FVector3(x1, y1, z1);
		}
		else
		{
			RetVal = FVector3(x2, y2, z2);
		}
	}

	return RetVal;
}
