#pragma once

class AActor;
class APlayer;
class FWorld : public TSharedFromThis<FWorld>
{
public:
	FWorld();
	virtual ~FWorld();

	TSharedPtr<FWorld> GetWorld() { return AsShared(); }

	void Tick();

	void DoTask(CallableType&& InCallable) const;

	bool Enter(TSharedPtr<AActor> InActor, bool bRandomPosition = true);
	bool Leave(TSharedPtr<AActor> InActor);

	bool EnterPlayer(TSharedPtr<APlayer> NewPlayer);
	bool LeavePlayer(TSharedPtr<APlayer> TargetPlayer);
	void MovePlayer(Protocol::C_MOVE InPacket);

private:
	bool AddActor(TSharedPtr<AActor> NewActor);
	bool RemoveActor(uint64 TargetActorId);
	void Broadcast(FSendBufferRef SendBuffer, uint64 ExceptId = 0);	// 자신은 제외하는 경우가 대다수이므로

private:
	TMap<uint64, TSharedPtr<AActor>> Actors;	// 해당 World에 존재하는 플레이어 목록(id로 관리)
	TSharedPtr<FTaskQueue> Tasks;
};

extern TSharedPtr<FWorld> GWorld;	// 전역 World 1개만 들고 있다고 가정(입장/퇴장만 테스트)