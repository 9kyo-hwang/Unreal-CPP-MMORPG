#include "pch.h"
#include "CameraMovement.h"

#include "InputManager.h"
#include "TimeManager.h"
#include "Transform.h"

CameraMovementComponent::CameraMovementComponent()
	: Speed(100.f)
{
}

CameraMovementComponent::~CameraMovementComponent()
{
}

void CameraMovementComponent::LateUpdate()
{
	shared_ptr<Transform> Transform = GetTransform();	// 이 스크립트가 붙은 오브젝트의 Transform
	float DeltaTime = TimeManager::Get()->GetDeltaTime();

	FVector3 Position = Transform->GetLocalPosition();

	if (InputManager::Get()->GetButton(EKeyCode::W))
	{
		Position += Transform->GetForward() * Speed * DeltaTime;
	}
	if ( InputManager::Get()->GetButton(EKeyCode::S) )
	{
		Position -= Transform->GetForward() * Speed * DeltaTime;
	}
	if ( InputManager::Get()->GetButton(EKeyCode::A) )
	{
		Position -= Transform->GetRight() * Speed * DeltaTime;
	}
	if ( InputManager::Get()->GetButton(EKeyCode::D) )
	{
		Position += Transform->GetRight() * Speed * DeltaTime;
	}

	if (InputManager::Get()->GetButton(EKeyCode::Q))
	{
		FVector3 Rotation = Transform->GetLocalRotation();
		Rotation.x += DeltaTime * 0.5f;
		Transform->SetLocalRotation(Rotation);
	}
	if ( InputManager::Get()->GetButton(EKeyCode::E) )
	{
		FVector3 Rotation = Transform->GetLocalRotation();
		Rotation.x -= DeltaTime * 0.5f;
		Transform->SetLocalRotation(Rotation);
	}

	if ( InputManager::Get()->GetButton(EKeyCode::Z) )
	{
		FVector3 Rotation = Transform->GetLocalRotation();
		Rotation.y -= DeltaTime * 0.5f;
		Transform->SetLocalRotation(Rotation);
	}
	if ( InputManager::Get()->GetButton(EKeyCode::C) )
	{
		FVector3 Rotation = Transform->GetLocalRotation();
		Rotation.y += DeltaTime * 0.5f;
		Transform->SetLocalRotation(Rotation);
	}

	Transform->SetLocalPosition(Position);
}
