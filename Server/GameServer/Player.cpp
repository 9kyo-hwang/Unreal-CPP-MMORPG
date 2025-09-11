#include "pch.h"
#include "Player.h"

APlayer::APlayer(int64 Id, FGameSessionRef InSession)
	: Super()
{
	ActorData->set_actor_id(Id);
	Session = InSession;
}

APlayer::~APlayer()
{
}
