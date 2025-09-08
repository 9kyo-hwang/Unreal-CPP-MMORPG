#pragma once

class APlayer;
class FWorld : public TSharedFromThis<FWorld>
{
public:
	FWorld();
	virtual ~FWorld();

	bool EnterPlayer(TSharedPtr<APlayer> NewPlayer);
	bool LeavePlayer(TSharedPtr<APlayer> TargetPlayer);
	void MovePlayer(const Protocol::C_MOVE& InPacket);

private:
	bool EnterPlayerHelper(TSharedPtr<APlayer> NewPlayer);
	bool LeavePlayerHelper(uint64 TargetPlayerId);
	void Broadcast(FSendBufferRef SendBuffer, uint64 ExceptId = 0);	// 자신은 제외하는 경우가 대다수이므로


private:
	FCriticalSection CriticalSection;	// USE LOCK
	TMap<uint64, TSharedPtr<APlayer>> Players;	// 해당 World에 존재하는 플레이어 목록(id로 관리)
};

extern TSharedPtr<FWorld> GWorld;	// 전역 World 1개만 들고 있다고 가정(입장/퇴장만 테스트)