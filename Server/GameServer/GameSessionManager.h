#pragma once

class FGameSession;

using FGameSessionRef = shared_ptr<FGameSession>;

class FGameSessionManager
{
public:
	void Add(FGameSessionRef NewSession);
	void Remove(FGameSessionRef TargetSession);
	void Broadcast(FSendBufferRef InSendBuffer);

private:
	FCriticalSection CriticalSection;
	set<FGameSessionRef> Sessions;
};

extern FGameSessionManager GSessionManager;
