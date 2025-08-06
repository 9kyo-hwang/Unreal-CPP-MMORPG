#pragma once
#include "AsyncTaskManager.h"

class UPlayer;

class AGameModeBase : public FAsyncTaskQueue
{
public:
	void Login(shared_ptr<UPlayer> NewPlayer);
	void Logout(shared_ptr<UPlayer> Exiting);
	void Broadcast(shared_ptr<FSendBuffer> SendBuffer);

private:
	TMap<uint64, shared_ptr<UPlayer>> Players;
	// FTaskArchive를 상속받아 자동으로 TaskQueue를 들고 있음
};

extern shared_ptr<AGameModeBase> GGameMode;