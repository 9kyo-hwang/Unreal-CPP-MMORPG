#pragma once

class FGameSession;

using FGameSessionRef = TSharedPtr<FGameSession>;

class FGameSessionManager
{
public:
	void Add(FGameSessionRef NewSession);
	void Remove(FGameSessionRef TargetSession);
	void Broadcast(FSendBufferRef InSendBuffer);

private:
	FCriticalSection CriticalSection;
	TSet<FGameSessionRef> Sessions;
};

extern FGameSessionManager GSessionManager;
