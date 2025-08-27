#pragma once

class ActorFactory
{
public:
	static TSharedPtr<class APlayer> SpawnPlayer(FGameSessionRef InSession);

private:
	static TAtomic<int64> IdGenerator;
};

