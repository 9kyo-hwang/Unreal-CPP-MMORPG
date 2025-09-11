#pragma once
#include "Pawn.h"
class AMonster : public APawn
{
	using Super = APawn;

public:
	AMonster();
	~AMonster() override;
};

