#include "pch.h"
#include "Pawn.h"

APawn::APawn()
{
	ActorData->set_actor_type(Protocol::EActorType::ACTOR_TYPE_PAWN);
}

APawn::~APawn()
{
}
