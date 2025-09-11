#include "pch.h"
#include "Player.h"

APlayer::APlayer(int64 Id, FGameSessionRef InSession)
{
	ActorData->set_actor_id(Id);
	Position->set_actor_id(Id);
	Session = InSession;
	bIsPlayer = true;
}

APlayer::~APlayer()
{
}
