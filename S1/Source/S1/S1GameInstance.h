// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S1.h"
#include "Engine/GameInstance.h"
#include "S1GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 특정 시점에서 서버에 연결할 수 있도록
	UFUNCTION(BlueprintCallable)
	void Connect();

	UFUNCTION(BlueprintCallable)
	void Disconnect();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(FSendBufferRef SendBuffer);

public:
	/**
	 *	별도의 세션 개념 없이, 해당 게임 인스턴스 자체를 세션처럼 다뤄서 서버와 통신할 수도 있음(포폴이면 그래도 됨)
	 *	하지만 Lobby Server, InGame Server와 같이 여러 서버와 통신할 수도 있어서 대리자인 세션을 두는 것이 일반적
	 */

	// 게임 서버와 통신할 소켓, 주소, 포트 번호
	FSocket* Socket;
	FString AddressString = TEXT("127.0.0.1");
	int16 Port = 7777;
	TSharedPtr<class FPacketSession> GameServerSession;
};
