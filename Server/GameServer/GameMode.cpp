#include "pch.h"
#include "GameMode.h"

#include "ClientSession.h"
#include "Player.h"

shared_ptr<AGameModeBase> GGameMode = MakeShared<AGameModeBase>();

void AGameModeBase::Login(shared_ptr<UPlayer> NewPlayer)
{
	Players.emplace(NewPlayer->PlayerId, NewPlayer);
}

void AGameModeBase::Logout(shared_ptr<UPlayer> Exiting)
{
	Players.erase(Exiting->PlayerId);
}

void AGameModeBase::Broadcast(shared_ptr<FSendBuffer> SendBuffer)
{
	for (const auto& Player : Players | views::values)
	{
		Player->OwnerSession->Send(SendBuffer);
	}
}

void AGameModeBase::Flush()
{
	while (true)
	{
		auto Task = Queue.Dequeue();
		if (Task == nullptr)
		{
			break;
		}

		Task->Launch();
	}
}