#include "pch.h"
#include "ActorFactory.h"
#include "Player.h"
#include "GameSession.h"

TAtomic<int64> ActorFactory::IdGenerator = 1;

TSharedPtr<APlayer> ActorFactory::SpawnPlayer(FGameSessionRef InSession)
{
	const int64 NewId = IdGenerator.fetch_add(1);

	TSharedPtr<APlayer> Player = MakeShared<APlayer>(NewId, InSession);
	InSession->ChangePlayer(Player);
	return Player;
}
