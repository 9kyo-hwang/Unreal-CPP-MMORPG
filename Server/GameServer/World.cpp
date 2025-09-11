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

bool FWorld::Enter(TSharedPtr<AActor> InActor, bool bRandomPosition)
{
	bool bResult = AddActor(InActor);
	if (bRandomPosition)
	{
		InActor->GetPosition()->set_x(FMath::RandRange(0.f, 500.f));
		InActor->GetPosition()->set_y(FMath::RandRange(0.f, 500.f));
		InActor->GetPosition()->set_z(100.f);
		InActor->GetPosition()->set_yaw(FMath::RandRange(0.f, 500.f));
	}

	// 모든 Actor가 Player라는 보장이 없어 DynamicCast로 safe-cast
	if (TSharedPtr<APlayer> Player = DynamicCastSharedPtr<APlayer>(InActor))
	{
		Protocol::S_ENTER_GAME Packet;
		Packet.set_success(bResult);

		Protocol::ActorData* ActorData = new Protocol::ActorData();
		ActorData->CopyFrom(*Player->GetActorData());
		Packet.set_allocated_actor_data(ActorData);

		// 만약 Copy가 아닌 InPlayer의 데이터 포인터를 복사해서 넘겨줬다면,
		// InPlayer의 해제에 대비하기 위해 release_player() 메서드를 호출해줘야 함
		if (FGameSessionRef Session = Player->GetSession())
		{
			Session->Send(ServerPacketHandler::MakeSendBuffer(Packet));
		}
	}

	{
		Protocol::S_SPAWN Packet;
		Protocol::ActorData* ActorData = Packet.add_players();  // 한 명만 소환 중
		ActorData->CopyFrom(*InActor->GetActorData());

		Broadcast(
			ServerPacketHandler::MakeSendBuffer(Packet),
			InActor->GetActorData()->actor_id()
		);
	}

	if (TSharedPtr<APlayer> Player = DynamicCastSharedPtr<APlayer>(InActor))
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

		if (FGameSessionRef Session = Player->GetSession())
		{
			Session->Send(ServerPacketHandler::MakeSendBuffer(Packet));
		}
	}

	return bResult;
}

bool FWorld::Leave(TSharedPtr<AActor> InActor)
{
	if (!InActor)
	{
		return false;
	}

	const uint64 ActorId = InActor->GetActorData()->actor_id();
	bool bResult = RemoveActor(ActorId);

	if (TSharedPtr<APlayer> Player = DynamicCastSharedPtr<APlayer>(InActor))
	{
		if (FGameSessionRef Session = Player->GetSession())
		{
			Protocol::S_LEAVE_GAME Packet;
			Session->Send(ServerPacketHandler::MakeSendBuffer(Packet));
		}
	}

	{
		Protocol::S_DESPAWN Packet;
		Packet.add_actor_ids(ActorId);

		auto SendBuffer = ServerPacketHandler::MakeSendBuffer(Packet);
		Broadcast(SendBuffer, ActorId);

		// LeavePlayerHelper에서 TargetPlayer가 Actors 맵에서 제거된 상태
		// 따라서 Broadcast를 통해서는 TargetPlayer에게 Despawn을 알릴 수 없음
		if (TSharedPtr<APlayer> Player = DynamicCastSharedPtr<APlayer>(InActor))
		{
			if (FGameSessionRef Session = Player->GetSession())
			{
				Session->Send(SendBuffer);
			}
		}
	}

	return bResult;
}

bool FWorld::EnterPlayer(TSharedPtr<APlayer> NewPlayer)
{
	return Enter(NewPlayer, true);
}

bool FWorld::LeavePlayer(TSharedPtr<APlayer> TargetPlayer)
{
	return Leave(TargetPlayer);
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
	Actor->GetPosition()->CopyFrom(InPacket.position());

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