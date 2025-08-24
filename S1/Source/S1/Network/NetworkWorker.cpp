// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetworkWorker.h"

#include "PacketSession.h"
#include "Sockets.h"

FNetworkThread::FNetworkThread(FSocket* InSocket, TSharedPtr<FPacketSession> InSession)
	: Socket(InSocket)
	, Thread(nullptr)
	, bRunning(true)
	, SessionRef(InSession)
{
}

FNetworkThread::~FNetworkThread()
{
}

void FNetworkThread::Destroy()
{
	bRunning = false;
}

FRecvThread::FRecvThread(FSocket* InSocket, TSharedPtr<FPacketSession> InSession)
	: FNetworkThread(InSocket, InSession)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

FRecvThread::~FRecvThread()
{
}

bool FRecvThread::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Thread Init")));
	return true;
}

// 해당 스레드의 메인 함수
uint32 FRecvThread::Run()
{
	// Packet 조립 전담, 외부에서 엔진 종료 등으로 멈춰야 한다면 bRunning = false
	while (bRunning)
	{
		TArray<uint8> Packet;
		if (Recv(Packet))
		{
			// 여기서 바로 메인 스레드에 접근하면 크래시 -> Queue에 넣어놓고 처리
			// SharedPtr이라 참조 횟수가 1 증가해 Session이 없어지는 위험은 없음
			if (FPacketSessionRef Session = SessionRef.Pin())
			{
				Session->RecvPacketQueue.Enqueue(Packet);
			}
		}
	}

	return 0;
}

void FRecvThread::Exit()
{
	FRunnable::Exit();
}

bool FRecvThread::Recv(TArray<uint8>& OutPacket)
{
	// 1. Header 읽기
	constexpr int32 HeaderSize = sizeof(FPacketHeader);  // 4byte(2byte + 2byte)
	TArray<uint8> HeaderBuffer;
	HeaderBuffer.AddZeroed(HeaderSize);

	if (!Recv(HeaderBuffer.GetData(), HeaderSize))
	{
		return false;
	}

	// 2. Header 파싱(기존의 reinterpret_cast를 이용한 방식도 OK)
	FPacketHeader Header;
	{
		FMemoryReader Reader(HeaderBuffer);
		Reader << Header;
		UE_LOG(LogTemp, Log, TEXT("Recv Packet ID: %d, PacketSize: %d"), Header.PacketID, Header.PacketSize);
	}

	// 3. Payload 파싱
	const int32 PayloadSize = Header.PacketSize - HeaderSize;
	TArray<uint8> PayloadBuffer;
	PayloadBuffer.AddZeroed(PayloadSize);

	if (!Recv(PayloadBuffer.GetData(), PayloadSize))
	{
		return false;
	}

	OutPacket = HeaderBuffer;
	OutPacket.Append(PayloadBuffer);
	return true;
}

// Size 크기의 패킷이 올 때까지 대기, Results에 담아 반환
// UE의 메인 게임 스레드와 별도로 동작
bool FRecvThread::Recv(uint8* Results, int32 Size)
{
	// 반환값이 false거나 데이터 크기가 0 이하 -> 연결이 끊김
	uint32 PendingDataSize;
	if (!Socket->HasPendingData(PendingDataSize) || PendingDataSize <= 0)
	{
		return false;
	}

	int32 Offset = 0;
	while (Size > 0)
	{
		// Size만큼 읽으려고 시도, 진짜로 읽은 크기는 BytesRead
		int32 BytesRead = 0;
		Socket->Recv(Results + Offset, Size, BytesRead);
		check(BytesRead <= Size);

		// 읽은 크기가 0 이하라면 연결이 끊긴 것
		if (BytesRead <= 0)
		{
			return false;
		}

		// 실제로 읽은 만큼 오프셋 이동, 남은 크기 계산
		Offset += BytesRead;
		Size -= BytesRead;
	}

	return true;
}

FSendThread::FSendThread(FSocket* InSocket, TSharedPtr<FPacketSession> InSession)
	: FNetworkThread(InSocket, InSession)
{
	Thread = FRunnableThread::Create(this, TEXT("SendWorkerThread"));
}

FSendThread::~FSendThread()
{
}

bool FSendThread::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Send Thread Init")));
	return true;
}

uint32 FSendThread::Run()
{
	// RecvThread에서는 RecvQueue에 패킷을 Enqueue
	// SendThread에서는 SendQueue로부터 버퍼를 Dequeue
	while (bRunning)
	{
		FSendBufferRef SendBuffer;
		if (FPacketSessionRef Session = SessionRef.Pin())
		{
			if (Session->SendPacketQueue.Dequeue(SendBuffer))
			{
				Send(SendBuffer);
			}
		}
	}

	return 0;
}

void FSendThread::Exit()
{
	FRunnable::Exit();
}

bool FSendThread::Send(FSendBufferRef SendBuffer)
{
	return Send(SendBuffer->GetData(), SendBuffer->GetWriteSize());
}

bool FSendThread::Send(const uint8* Buffer, int32 Size)
{
	while (Size > 0)
	{
		int32 BytesSent = 0;
		if (!Socket->Send(Buffer, Size, BytesSent))
		{
			return false;
		}

		Size -= BytesSent;
		Buffer += BytesSent;
	}

	return true;
}
