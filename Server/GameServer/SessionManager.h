#pragma once

class FGameSession;

class FSessionManager
{
public:
	void Add(shared_ptr<FGameSession> Session);
	void Remove(shared_ptr<FGameSession> Session);
	void Broadcast(shared_ptr<FSendBuffer> SendBuffer);

private:
	USE_LOCK;
	TSet<shared_ptr<FGameSession>> Sessions;
};

extern FSessionManager GSessionManager;