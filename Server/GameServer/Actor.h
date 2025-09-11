#pragma once

namespace Protocol
{
	class ActorData;
}

class FWorld;

class AActor : public TSharedFromThis<AActor>
{
public:
	AActor();
	virtual ~AActor();

	Protocol::ActorData* GetActorData() const { return ActorData; }
	Protocol::PositionData* GetPosition() const { return Position; }
	bool IsPlayer() const { return bIsPlayer; }

	TSharedPtr<FWorld> GetWorld() const { return World.load().lock(); }
	void SetWorld(TSharedPtr<FWorld> NewWorld) { World.store(NewWorld); }
	void SetWorld(TWeakPtr<FWorld> NewWorld) { World.store(NewWorld); }

protected:
	Protocol::ActorData* ActorData;
	Protocol::PositionData* Position;
	TAtomic<TWeakPtr<FWorld>> World;	// Room은 바뀔 가능성이 있음. 이 때 객체 포인터와 카운팅 블록을 한 번에 수정하지 않아 멀티 스레드에 취약 -> Atomic
	bool bIsPlayer;
};

