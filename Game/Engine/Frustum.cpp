#include "pch.h"
#include "Frustum.h"

#include "CameraComponent.h"

void Frustum::FinalUpdate()
{
	FMatrix InverseViewMatrix = UCameraComponent::StaticViewMatrix.Invert();
	FMatrix InverseProjectionMatrix = UCameraComponent::StaticProjectionMatrix.Invert();
	FMatrix InverseMatrix = InverseProjectionMatrix * InverseViewMatrix;

	// x, y, z, w(1)
	// 사이의 Flip Space로 넘기기 위함
	vector<FVector3> WorldPositions
	{
		::XMVector3TransformCoord(FVector3(-1.f, 1.f, 0.f), InverseMatrix),
		::XMVector3TransformCoord(FVector3(1.f, 1.f, 0.f), InverseMatrix),
		::XMVector3TransformCoord(FVector3(1.f, -1.f, 0.f), InverseMatrix),
		::XMVector3TransformCoord(FVector3(-1.f, -1.f, 0.f), InverseMatrix),
		::XMVector3TransformCoord(FVector3(-1.f, 1.f, 1.f), InverseMatrix),
		::XMVector3TransformCoord(FVector3(1.f, 1.f, 1.f), InverseMatrix),
		::XMVector3TransformCoord(FVector3(1.f, -1.f, 1.f), InverseMatrix),
		::XMVector3TransformCoord(FVector3(-1.f, -1.f, 1.f), InverseMatrix)
	};

	Planes[StaticCast<uint8>(EPlaneType::Front)] = ::XMPlaneFromPoints(WorldPositions[0], WorldPositions[1], WorldPositions[2]);		// CW
	Planes[StaticCast<uint8>( EPlaneType::Back)] = ::XMPlaneFromPoints(WorldPositions[4], WorldPositions[7], WorldPositions[5]);		// CCW
	Planes[StaticCast<uint8>( EPlaneType::Up)] = ::XMPlaneFromPoints(WorldPositions[4], WorldPositions[5], WorldPositions[1]);		// CW
	Planes[StaticCast<uint8>( EPlaneType::Down)] = ::XMPlaneFromPoints(WorldPositions[7], WorldPositions[3], WorldPositions[6]);		// CCW
	Planes[StaticCast<uint8>( EPlaneType::Left)] = ::XMPlaneFromPoints(WorldPositions[4], WorldPositions[0], WorldPositions[7]);		// CW
	Planes[StaticCast<uint8>( EPlaneType::Right)] = ::XMPlaneFromPoints(WorldPositions[5], WorldPositions[6], WorldPositions[1]);	// CCW
}

bool Frustum::ContainsSphere(const FVector3& Position, float Radius)
{
	// TODO: BoundingFrustum 공식 제공, Quaternion 이용

	for (const FVector4& FrustumPlane : Planes)
	{
		FVector3 Normal = FVector3(FrustumPlane.x, FrustumPlane.y, FrustumPlane.z);	// N: (a, b, c)
		if (Normal.Dot(Position) + FrustumPlane.w > Radius)	// (ax + by + cz) + d > radius
		{
			return false;
		}
	}

	return true;
}
