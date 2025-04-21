#pragma once
#include "ActorComponent.h"

class UCameraMovementComponent : public UActorComponent
{
	using Super = UActorComponent;

public:
	UCameraMovementComponent();
	~UCameraMovementComponent() override;

	void LateUpdate(float DeltaTime) override;

private:
	float Speed;
	FVector2 PrevPosition;
};

