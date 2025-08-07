#pragma once
#include "AsyncTaskQueue.h"

class UPlayer;

class AGameModeBase : public FAsyncTaskQueue
{
public:
	void Login(shared_ptr<UPlayer> NewPlayer);
	void Logout(shared_ptr<UPlayer> Exiting);
	void Broadcast(shared_ptr<FSendBuffer> SendBuffer);

private:
	TMap<uint64, shared_ptr<UPlayer>> Players;
};

extern shared_ptr<AGameModeBase> GGameMode;