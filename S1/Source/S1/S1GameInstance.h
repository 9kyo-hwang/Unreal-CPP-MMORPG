// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S1.h"
#include "Engine/GameInstance.h"
#include "S1GameInstance.generated.h"

class AS1Player;

namespace Protocol
{
	class ActorData;
	class S_SPAWN;
	class S_ENTER_GAME;
	class S_DESPAWN;
	class S_MOVE;
}

/**
 * 
 */
UCLASS()
class S1_API US1GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Connect();

	UFUNCTION(BlueprintCallable)
	void Disconnect();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(FSendBufferRef SendBuffer);

	void SpawnPlayer(const Protocol::ActorData& InActorData, bool bIsMyPlayer);
	void SpawnPlayer(const Protocol::S_ENTER_GAME& InPacket);
	void SpawnPlayer(const Protocol::S_SPAWN& InPacket);

	void DespawnPlayer(const uint64 ActorId);
	void DespawnPlayer(const Protocol::S_DESPAWN& InPacket);

	void MovePlayer(const Protocol::S_MOVE& InPacket);

public:
	FSocket* Socket;
	FString AddressString = TEXT("127.0.0.1");
	int16 Port = 7777;
	TSharedPtr<FPacketSession> GameServerSession;

	UPROPERTY(EditAnywhere)	// BP에서 플레이어 클래스 세팅 가능
	TSubclassOf<AS1Player> OtherPlayerClass;
	TObjectPtr<AS1Player> MyPlayer;
	TMap<uint64, TObjectPtr<AS1Player>> Players;
};
