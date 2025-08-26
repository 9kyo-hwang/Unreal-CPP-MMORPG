// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/PacketSession.h"

#include "ClientPacketHandler.h"
#include "NetworkWorker.h"

FPacketSession::FPacketSession(FSocket* InSocket)
	: Socket(InSocket)
{
	// 세션 생성자에서 패킷 핸들러 초기화
	ClientPacketHandler::Init();
}

FPacketSession::~FPacketSession()
{
	Disconnect();
}

// Queue에 담긴 패킷은 UE의 GameThread가 처리하게 됨
void FPacketSession::HandleRecvPackets()
{
	while (true)
	{
		TArray<uint8> Packet;  // PacketHeader 포함
		if (!RecvPacketQueue.Dequeue(Packet))
		{
			break;
		}

		// 현재 FPacketSessionRef의 &로 넘겨주고 있어서, 임시 변수에 할당
		FPacketSessionRef ThisPtr = AsShared();
		ClientPacketHandler::HandlePacket(ThisPtr, Packet.GetData(), Packet.Num());
	}
}

void FPacketSession::SendPacket(FSendBufferRef SendBuffer)
{
	SendPacketQueue.Enqueue(SendBuffer);
}

void FPacketSession::Run()
{
	// Socket의 Recv, Send를 여기서 수행하게 될 것
	// 단 Blocking 방식이기 때문에 WorkerThread를 통해 동작하도록 작성해야 함
	// 다시 말해 서버로부터 송수신하는 패킷을 전담해서 모으는 별도의 클래스가 존재할 예정
	// 여기서는 Recv, Send할 준비가 완료된 것들을 해당 클래스로부터 받아와서 호출할 것

	RecvThread = MakeShared<FRecvThread>(Socket, AsShared());
	SendThread = MakeShared<FSendThread>(Socket, AsShared());
}

void FPacketSession::Disconnect()
{
	if (RecvThread)
	{
		RecvThread->Destroy();  // 네트워크 스레드를 반드시 중단시켜줘야 함
		RecvThread = nullptr;
	}

	if (SendThread)
	{
		SendThread->Destroy();  // 안그러면 엔진 종료 후 재시작 시 크래시 발생
		SendThread = nullptr;
	}
}
