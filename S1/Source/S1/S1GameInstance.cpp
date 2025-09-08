// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GameInstance.h"

#include "ClientPacketHandler.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "S1Player.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Kismet/GameplayStatics.h"

void US1GameInstance::Connect()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address Address;
	FIPv4Address::Parse(AddressString, Address);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Address.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

	if (Socket->Connect(*InternetAddr))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

		GameServerSession = MakeShared<FPacketSession>(Socket);
		GameServerSession->Run();	// Create RecvThread

		// TEMP: Lobby에서 캐릭터 선택창을 표시하는 등 Login Packet 전송
		Protocol::C_LOGIN Packet;
		SendPacket(ClientPacketHandler::MakeSendBuffer(Packet));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void US1GameInstance::Disconnect()
{
	if (!Socket || !GameServerSession)
	{
		return;
	}

	Protocol::C_LEAVE_GAME Packet;
	GameServerSession->SendPacket(ClientPacketHandler::MakeSendBuffer(Packet));
}

void US1GameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	GameServerSession->HandleRecvPackets();
}

void US1GameInstance::SendPacket(FSendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	GameServerSession->SendPacket(SendBuffer);
}

void US1GameInstance::SpawnPlayer(const Protocol::PlayerInfo& InPlayerInfo, bool bIsMyPlayer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		const uint64 ObjectId = InPlayerInfo.object_id();
		if (!Players.Contains(ObjectId))
		{
			FVector SpawnLocation(InPlayerInfo.x(), InPlayerInfo.y(), InPlayerInfo.z());
			if (bIsMyPlayer)
			{
				const APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
				if (AS1Player* Player = Cast<AS1Player>(PC->GetPawn()))
				{
					Player->SetPosition(InPlayerInfo);
					MyPlayer = Player;
					Players.Emplace(ObjectId, Player);
				}
			}
			else
			{
				AS1Player* OtherPlayer = Cast<AS1Player>(World->SpawnActor(OtherPlayerClass, &SpawnLocation));
				OtherPlayer->SetPosition(InPlayerInfo);
				Players.Emplace(ObjectId, OtherPlayer);
			}
		}
	}
}

void US1GameInstance::SpawnPlayer(const Protocol::S_ENTER_GAME& InPacket)
{
	SpawnPlayer(InPacket.player(), true);
}

void US1GameInstance::SpawnPlayer(const Protocol::S_SPAWN& InPacket)
{
	for (auto& PlayerInfo : InPacket.players())
	{
		SpawnPlayer(PlayerInfo, false);
	}
}

void US1GameInstance::DespawnPlayer(const uint64 ObjectId)
{
	if (!Socket || !GameServerSession)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (TObjectPtr<AS1Player>* TargetPlayerPtr = Players.Find(ObjectId))
		{
			World->DestroyActor(*TargetPlayerPtr);
		}
	}
}

void US1GameInstance::DespawnPlayer(const Protocol::S_DESPAWN& InPacket)
{
	for (const uint64 ObjectId : InPacket.object_ids())
	{
		DespawnPlayer(ObjectId);
	}
}

void US1GameInstance::MovePlayer(const Protocol::S_MOVE& InPacket)
{
	if (!Socket || !GameServerSession)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		const int64 ObjectId = InPacket.info().object_id();
		if (const TObjectPtr<AS1Player>* PlayerPtr = Players.Find(ObjectId))
		{
			if (const TObjectPtr<AS1Player> Player = *PlayerPtr; !Player->IsMyPlayer())
			{
				Player->SetPosition(InPacket.info());
			}
		}
	}
}
