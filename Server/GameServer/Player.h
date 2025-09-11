#pragma once
#include "Pawn.h"

// In-game Player
class FWorld;
class APlayer : public APawn
{
	using Super = APawn;

public:
	APlayer(int64 Id, FGameSessionRef InSession);
	~APlayer() override;
	FGameSessionRef GetSession() const { return Session.lock(); }

protected:
	TWeakPtr<FGameSession> Session;	// Session을 알고 있어야 패킷을 전송할 수 있음
};
