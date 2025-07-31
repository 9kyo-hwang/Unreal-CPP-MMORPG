#include "pch.h"
#include "ServerPacketHandler.h"

#include "BufferReader.h"
#include "BufferWriter.h"
#include "Session.h"

void ServerPacketHandler::HandlePacket(BYTE* Buffer, int32 Length)
{
	FBufferReader br(Buffer, Length);
	FPacketHeader PacketHeader; br.Peek(&PacketHeader);
	printf("ID: %d\tSize: %d\n", PacketHeader.Id, PacketHeader.Size);

	// 현재는 Client -> Server 패킷이 없어 별도의 처리 없음
	switch (PacketHeader.Id)
	{
	default:
		break;
	}
}

shared_ptr<FSendBuffer> ServerPacketHandler::CreatePacket_Test(uint64 Id, uint32 Hp, uint16 Attack, TArray<FBuffData> Buffs, FString Name)
{
	shared_ptr<FSendBuffer> SendBuffer = GSendBufferPool->Open(4096);

	FBufferWriter Writer(SendBuffer->GetData(), SendBuffer->GetCapacity());
	FPacketHeader* PacketHeader = Writer.Reserve<FPacketHeader>();

	Writer << Id << Hp << Attack;

	struct FListHeader
	{
		uint16 Offset;
		uint16 Num;
	};

	// 가변 길이 배열의 크기를 먼저 기록(uint16으로 제한해도(최대 약 6만개) 충분)
	FListHeader* BuffsHeader = Writer.Reserve<FListHeader>();
	BuffsHeader->Num = Buffs.size();
	BuffsHeader->Offset = Writer.GetWriteSize();	// 고정 데이터를 다 쓴 시점의 쓰기 크기가 곧 가변 데이터 작성 시작(오프셋) 위치
	for (FBuffData& Buff : Buffs)
	{
		Writer << Buff.Id << Buff.RemainTime;
	}
		
	PacketHeader->Size = Writer.GetWriteSize();
	PacketHeader->Id = EPacketId::Test;	// TODO: Protocol Id

	SendBuffer->Close(Writer.GetWriteSize());

	return SendBuffer;
}
