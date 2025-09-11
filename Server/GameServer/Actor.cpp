#include "pch.h"
#include "Actor.h"

AActor::AActor()
	: ActorData(new Protocol::ActorData())
	, Position(new Protocol::PositionData())
	, bIsPlayer(false)
{
	ActorData->set_allocated_position(Position);
}

AActor::~AActor()
{
	delete Position;
	delete ActorData;
}
