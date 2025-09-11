#include "pch.h"
#include "World.h"

#include <ranges>

#include "GameSession.h"
#include "MathUtility.h"
#include "Player.h"

TSharedPtr<FWorld> GWorld = MakeShared<FWorld>();

FWorld::FWorld()
{
	Tasks = MakeShared<FTaskQueue>();
}

FWorld::~FWorld()
{
}

void FWorld::Tick()
{
	// cout << "[FWorld::Tick]\n";

	// 0.1초 단위로 Tick을 반복 호출
	Tasks->PostAfter(100, GetWorld(), &FWorld::Tick);
}

void FWorld::DoTask(CallableType&& InCallable) const
{
	Tasks->Post(std::move(InCallable));
}

bool FWorld::EnterPlayer(TSharedPtr<APlayer> NewPlayer)
{
	bool bResult = AddActor(NewPlayer);

	NewPlayer->GetPosition()->set_x(FMath::RandRange(0.f, 500.f));
	NewPlayer->GetPosition()->set_y(FMath::RandRange(0.f, 500.f));
	NewPlayer->GetPosition()->set_z(100.f);
	NewPlayer->GetPosition()->set_yaw(FMath::RandRange(0.f, 500.f));

	// World에 입장 및 플레이어 Spawn이 완료되었으므로
	// 1. 패킷을 보내온 클라이언트에게 답신을 해야 함
	{
		Protocol::S_ENTER_GAME Packet;
		Packet.set_success(bResult);
		
		Protocol::ActorData* ActorData = new Protocol::ActorData();
		ActorData->CopyFrom(*NewPlayer->GetActorData());
		Packet.set_allocated_actor_data(ActorData);

		// 만약 Copy가 아닌 InPlayer의 데이터 포인터를 복사해서 넘겨줬다면,
		// InPlayer의 해제에 대비하기 위해 release_player() 메서드를 호출해줘야 함
		if (FGameSessionRef Session = NewPlayer->GetSession())
		{
			Session->Send(ServerPacketHandler::MakeSendBuffer(Packet));
		}
	}

	// 2. World에 속한 다른 클라이언트에게도 알려야 함
	{
		Protocol::S_SPAWN Packet;
		Protocol::ActorData* ActorData = Packet.add_players();  // 한 명만 소환 중
		ActorData->CopyFrom(*NewPlayer->GetActorData());

		Broadcast(
			ServerPacketHandler::MakeSendBuffer(Packet),
			NewPlayer->GetActorData()->actor_id()
		);
	}

	// 3. 기존에 입장한 플레이어 리스트를 새로 입장한 플레이어에게 알려야 함
	{
		Protocol::S_SPAWN Packet;

		for (const auto& Actor : Actors | views::values)
		{
			if (Actor->IsPlayer())
			{
				Protocol::ActorData* ActorData = Packet.add_players();
				ActorData->CopyFrom(*Actor->GetActorData());
			}
		}

		if (FGameSessionRef Session = NewPlayer->GetSession())
		{
			Session->Send(ServerPacketHandler::MakeSendBuffer(Packet));
		}
	}

	return bResult;
}

bool FWorld::LeavePlayer(TSharedPtr<APlayer> TargetPlayer)
{
	if (!TargetPlayer)
	{
		return false;
	}

	const uint64 TargetPlayerId = TargetPlayer->GetActorData()->actor_id();
	bool bResult = RemoveActor(TargetPlayerId);

	// 퇴장하는 플레이어에게 퇴장 사실을 알림
	{
		if (FGameSessionRef Session = TargetPlayer->GetSession())
		{
			Protocol::S_LEAVE_GAME Packet;
			Session->Send(ServerPacketHandler::MakeSendBuffer(Packet));
		}
	}

	// 월드 내 다른 플레이어들에게 해당 플레이어가 Despawn 됨을 알림
	{
		Protocol::S_DESPAWN Packet;
		Packet.add_actor_ids(TargetPlayerId);

		auto SendBuffer = ServerPacketHandler::MakeSendBuffer(Packet);
		Broadcast(SendBuffer, TargetPlayerId);

		// LeavePlayerHelper에서 TargetPlayer가 Actors 맵에서 제거된 상태
		// 따라서 Broadcast를 통해서는 TargetPlayer에게 Despawn을 알릴 수 없음
		if (FGameSessionRef Session = TargetPlayer->GetSession())
		{
			Session->Send(SendBuffer);
		}
	}

	return bResult;
}

void FWorld::MovePlayer(Protocol::C_MOVE InPacket)
{
	const int64 ActorId = InPacket.position().actor_id();
	if (!Actors.contains(ActorId))
	{
		return;
	}

	// TODO: Packet에 든 위치 정보의 Validation Check
	TSharedPtr<AActor> Actor = Actors.at(ActorId);
	Actor->GetActorData()->CopyFrom(InPacket.position());

	Protocol::S_MOVE Packet;
	Protocol::PositionData* Position = Packet.mutable_position();
	Position->CopyFrom(InPacket.position());

	Broadcast(ServerPacketHandler::MakeSendBuffer(Packet));
}

// World가 플레이어를 최종적으로 관리
// 따라서 해당 메서드가 실행되는 동안에는 다른 코드에서 Player에 접근할 수 없음을 보장
// => 별도의 Lock을 걸어주지 않을 것
bool FWorld::AddActor(TSharedPtr<AActor> NewActor)
{
	if (Actors.contains(NewActor->GetActorData()->actor_id()))
	{
		return false;
	}

	Actors.emplace(NewActor->GetActorData()->actor_id(), NewActor);
	NewActor->SetWorld(GetWorld());
	return true;
}

bool FWorld::RemoveActor(uint64 TargetActorId)
{
	if (!Actors.contains(TargetActorId))
	{
		return false;
	}

	Actors.at(TargetActorId)->SetWorld(TWeakPtr<FWorld>());	// like nullptr set
	Actors.erase(TargetActorId);

	return true;
}

void FWorld::Broadcast(FSendBufferRef SendBuffer, uint64 ExceptId)
{
	// Lock이 걸려 있는 상황에서 Send에 실패해 해당 World로부터 퇴출당하면 문제가 될 수 있음
	// 예전에 Server에서 일어났었던 비슷한 상황에서는 Player 정보들을 임시 객체에 복사해서 수행
	for (auto& [Id, Actor] : Actors)
	{
		if (Id == ExceptId)
		{
			continue;
		}

		if (TSharedPtr<APlayer> Player = DynamicCastSharedPtr<APlayer>(Actor))
		{
			if (FGameSessionRef Session = Player->GetSession())
			{
				Session->Send(SendBuffer);
			}
		}
	}
}