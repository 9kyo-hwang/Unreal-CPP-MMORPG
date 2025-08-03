#include "pch.h"
#include "GameMode.h"

#include "ClientSession.h"
#include "Player.h"

AGameModeBase GGameMode;

void AGameModeBase::Login(shared_ptr<UPlayer> NewPlayer)
{
	WRITE_LOCK;

	Players.emplace(NewPlayer->PlayerId, NewPlayer);
}

void AGameModeBase::Logout(shared_ptr<UPlayer> Exiting)
{
	WRITE_LOCK;

	Players.erase(Exiting->PlayerId);
}

void AGameModeBase::Broadcast(shared_ptr<FSendBuffer> SendBuffer)
{
	WRITE_LOCK;
	for (const auto& Player : Players | views::values)
	{
		Player->OwnerSession->Send(SendBuffer);
	}
}
