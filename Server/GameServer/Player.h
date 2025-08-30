#pragma once

// In-game Player
class FWorld;
class APlayer : public TSharedFromThis<APlayer>
{
public:
	APlayer(int64 Id, FGameSessionRef InSession);
	virtual ~APlayer();

	Protocol::PlayerInfo* GetInfo() const { return Info; }
	FGameSessionRef GetSession() const { return Session.lock(); }
	TSharedPtr<FWorld> GetWorld() const { return World.load().lock(); }
	void SetWorld(TSharedPtr<FWorld> NewWorld) { World.store(NewWorld); }
	void SetWorld(TWeakPtr<FWorld> NewWorld) { World.store(NewWorld); }

protected:
	Protocol::PlayerInfo* Info;
	TWeakPtr<FGameSession> Session;	// Session을 알고 있어야 패킷을 전송할 수 있음
	TAtomic<TWeakPtr<FWorld>> World;	// Room은 바뀔 가능성이 있음. 이 때 객체 포인터와 카운팅 블록을 한 번에 수정하지 않아 멀티 스레드에 취약 -> Atomic
};
