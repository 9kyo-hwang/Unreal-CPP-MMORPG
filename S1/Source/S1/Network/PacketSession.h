// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S1.h"

/**
 * 
 */

class FRecvThread;
class FSendThread;

class S1_API FPacketSession : public TSharedFromThis<FPacketSession>
{
public:
	FPacketSession(FSocket* InSocket);
	~FPacketSession();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	// SendBuffer가 Blueprintable이 아니기 때문에 BlueprintCallable 하면 안됨
	void SendPacket(FSendBufferRef SendBuffer);

	void Run();
	void Disconnect();

public:
	FSocket* Socket;

	TSharedPtr<FRecvThread> RecvThread;
	TSharedPtr<FSendThread> SendThread;

	// GameThread - NetworkThread가 데이터를 주고받는 공용 큐
	TQueue<TArray<uint8>> RecvPacketQueue;	// Thread-Safe w. Lock-Free
	TQueue<FSendBufferRef> SendPacketQueue;
};
