#pragma once

class UPlayer;

class AGameModeBase
{
public:
	void Login(shared_ptr<UPlayer> NewPlayer);
	void Logout(shared_ptr<UPlayer> Exiting);
	void Broadcast(shared_ptr<FSendBuffer> SendBuffer);

private:
	USE_LOCK;
	TMap<uint64, shared_ptr<UPlayer>> Players;
};

extern AGameModeBase GGameMode;