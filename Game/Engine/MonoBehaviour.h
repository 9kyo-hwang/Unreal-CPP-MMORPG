#pragma once
#include "ActorComponent.h"

class MonoBehaviour : public UActorComponent
{
	using Super = UActorComponent;

public:
	MonoBehaviour();
	~MonoBehaviour() override;

private:
	void FinalUpdate(float DeltaTime) sealed {}	// 더 이상 활용하지 못하도록 sealed
};

