#include "pch.h"
#include "World.h"

#include "GameSession.h"
#include "MathUtility.h"
#include "Player.h"

TSharedPtr<FWorld> GWorld = MakeShared<FWorld>();

FWorld::FWorld()
{
}

FWorld::~FWorld()
{
}

bool FWorld::EnterPlayer(TSharedPtr<APlayer> InPlayer)
{
	FScopeLock ScopeLock(CriticalSection);

	bool bResult = EnterPlayerHelper(InPlayer);

	InPlayer->GetData()->set_x(FMath::RandRange(0.f, 500.f));
	InPlayer->GetData()->set_y(FMath::RandRange(0.f, 500.f));
	InPlayer->GetData()->set_z(FMath::RandRange(0.f, 500.f));
	InPlayer->GetData()->set_yaw(FMath::RandRange(0.f, 500.f));

	// World에 입장 및 플레이어 Spawn이 완료되었으므로
	// 1. 패킷을 보내온 클라이언트에게 답신을 해야 함
	{
		Protocol::S_ENTER_GAME Packet;
		Packet.set_success(bResult);

		Protocol::PlayerInfo* Info = new Protocol::PlayerInfo;
		Info->CopyFrom(*InPlayer->GetData());
		Packet.set_allocated_player(Info);

		// 만약 Copy가 아닌 InPlayer의 데이터 포인터를 복사해서 넘겨줬다면,
		// InPlayer의 해제에 대비하기 위해 release_player() 메서드를 호출해줘야 함

		if (FGameSessionRef Session = InPlayer->GetSession())
		{
			Session->Send(ServerPacketHandler::MakeSendBuffer(Packet));
		}
	}

	// 2. World에 속한 다른 클라이언트에게도 알려야 함
	{
		
	}

	return bResult;
}

// World가 플레이어를 최종적으로 관리
// 따라서 해당 메서드가 실행되는 동안에는 다른 코드에서 Player에 접근할 수 없음을 보장
// => 별도의 Lock을 걸어주지 않을 것
bool FWorld::EnterPlayerHelper(TSharedPtr<APlayer> InPlayer)
{
	if (Players.contains(InPlayer->GetData()->object_id()))
	{
		return false;
	}

	Players.emplace(InPlayer->GetData()->object_id(), InPlayer);
	InPlayer->SetWorldIn(AsShared());
	return true;
}
