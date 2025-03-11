#pragma once
#include "MonoBehaviour.h"

class CameraMovementComponent : public MonoBehaviour
{
	using Super = MonoBehaviour;

public:
	CameraMovementComponent();
	~CameraMovementComponent() override;

	void LateUpdate() override;

private:
	float Speed;
	FVector2 PrevPosition;
};

