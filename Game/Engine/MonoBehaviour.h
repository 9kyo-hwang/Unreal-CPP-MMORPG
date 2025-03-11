#pragma once
#include "Component.h"

class MonoBehaviour : public Component
{
	using Super = Component;

public:
	MonoBehaviour();
	~MonoBehaviour() override;

private:
	void FinalUpdate() sealed {}	// 더 이상 활용하지 못하도록 sealed
};

