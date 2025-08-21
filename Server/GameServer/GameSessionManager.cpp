#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

FGameSessionManager GSessionManager;

void FGameSessionManager::Add(FGameSessionRef NewSession)
{
	FScopeLock ScopeLock(CriticalSection);
	Sessions.insert(NewSession);
}

void FGameSessionManager::Remove(FGameSessionRef TargetSession)
{
	FScopeLock ScopeLock(CriticalSection);
	Sessions.erase(TargetSession);
}

void FGameSessionManager::Broadcast(FSendBufferRef InSendBuffer)
{
	FScopeLock ScopeLock(CriticalSection);
	for (FGameSessionRef Session : Sessions)
	{
		Session->Send(InSendBuffer);
	}
}