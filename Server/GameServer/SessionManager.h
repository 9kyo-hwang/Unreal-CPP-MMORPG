#pragma once

class FClientSession;

class FSessionManager
{
public:
	void Add(shared_ptr<FClientSession> Session);
	void Remove(shared_ptr<FClientSession> Session);
	void Broadcast(shared_ptr<FSendBuffer> SendBuffer);

private:
	USE_LOCK;
	TSet<shared_ptr<FClientSession>> Sessions;
};

extern FSessionManager GSessionManager;