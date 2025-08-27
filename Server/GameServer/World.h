#pragma once

class APlayer;
class FWorld : public TSharedFromThis<FWorld>
{
public:
	FWorld();
	virtual ~FWorld();

	bool EnterPlayer(TSharedPtr<APlayer> InPlayer);

private:
	bool EnterPlayerHelper(TSharedPtr<APlayer> InPlayer);

	FCriticalSection CriticalSection;	// USE LOCK

private:
	TMap<uint64, TSharedPtr<APlayer>> Players;	// 해당 World에 존재하는 플레이어 목록(id로 관리)
};

extern TSharedPtr<FWorld> GWorld;	// 전역 World 1개만 들고 있다고 가정(입장/퇴장만 테스트)