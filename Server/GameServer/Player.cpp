#include "pch.h"
#include "Player.h"

APlayer::APlayer(int64 Id, FGameSessionRef InSession)
	: Info(new Protocol::PlayerInfo())
{
	Info->set_object_id(Id);
	Session = InSession;
}

APlayer::~APlayer()
{
	delete Info;
}
