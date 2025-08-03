#include "pch.h"
#include "SessionManager.h"

#include "ClientSession.h"

FSessionManager GSessionManager;

void FSessionManager::Add(shared_ptr<FClientSession> Session)
{
	WRITE_LOCK;
	Sessions.insert(Session);
}

void FSessionManager::Remove(shared_ptr<FClientSession> Session)
{
	WRITE_LOCK;
	Sessions.erase(Session);
}

void FSessionManager::Broadcast(shared_ptr<FSendBuffer> SendBuffer)
{
	WRITE_LOCK;
	for (auto Session : Sessions)
	{
		Session->Send(SendBuffer);
	}
}