// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S1.h"

struct S1_API FPacketHeader
{
	FPacketHeader()
		: PacketSize(0)
		, PacketID(0)
	{}

	FPacketHeader(uint16 InPacketSize, uint16 InPacketID)
		: PacketSize(InPacketSize)
		, PacketID(InPacketID)
	{}

	friend FArchive& operator<<(FArchive& Ar, FPacketHeader& Header)
	{
		Ar << Header.PacketSize << Header.PacketID;
		return Ar;
	}

	uint16 PacketSize;  // (Header + Payload) Size
	uint16 PacketID;
};

class S1_API FNetworkThread : public FRunnable
{
public:
	FNetworkThread(FSocket* InSocket, TSharedPtr<FPacketSession> InSession);
	virtual ~FNetworkThread() override;

	virtual void Destroy();

protected:
	FSocket* Socket;
	FRunnableThread* Thread;
	bool bRunning;
	TWeakPtr<FPacketSession> SessionRef;
};

class FPacketSession;
class S1_API FRecvThread : public FNetworkThread
{
public:
	FRecvThread(FSocket* InSocket, TSharedPtr<FPacketSession> InSession);
	virtual ~FRecvThread() override;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

private:
	bool Recv(TArray<uint8>& OutPacket);
	bool Recv(uint8* Results, int32 Size);
};

class S1_API FSendThread : public FNetworkThread
{
public:
	FSendThread(FSocket* InSocket, TSharedPtr<FPacketSession> InSession);
	virtual ~FSendThread() override;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	bool Send(FSendBufferRef SendBuffer);

private:
	bool Send(const uint8* Buffer, int32 Size);
};