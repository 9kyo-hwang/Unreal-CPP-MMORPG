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

ServerPacketWriter_Test::ServerPacketWriter_Test(uint64 Id, uint32 Hp, uint16 Attack)
	: Packet(nullptr)
{
	SendBuffer = GSendBufferPool->Open(4096);
	Writer = FBufferWriter(SendBuffer->GetData(), SendBuffer->GetCapacity());

	Packet = Writer.Reserve<FPacket>();	// Cursor는 현재 가변 데이터를 담는 공간의 시작 주소를 가리킴
	Packet->PacketSize = 0;	// TODO
	Packet->PacketId = EPacketId::Test;
	Packet->Id = Id;
	Packet->Hp = Hp;
	Packet->Attack = Attack;
	Packet->BuffsOffset = 0;	// 아직 버프 데이터는 추가하지 않음
	Packet->NumBuff = 0;	// 버프 데이터가 없으므로 0으로 초기화
}

ServerPacketWriter_Test::FBuffDataArray ServerPacketWriter_Test::ReserveBuffs(uint16 NumBuff)
{
	// [BuffDataBuffDataBuffData][???][BuffDataBuffData]...
	// Buff 타입의 데이터만 동적으로 추가되는 것이 아님
	// 처음에 공간을 미리 원하는 만큼 할당해주고 시작해야 안전(추후 크기 기반으로 파싱하기 위해)
	FBuffData* BuffDataArrayBegin = Writer.Reserve<FBuffData>(NumBuff);	// id를 들고 있어 더 커지게 됨
	Packet->NumBuff = NumBuff;
	Packet->BuffsOffset = reinterpret_cast<uint64>(BuffDataArrayBegin) - reinterpret_cast<uint64>(Packet);
	return FBuffDataArray(BuffDataArrayBegin, NumBuff);
}

ServerPacketWriter_Test::FBuffDataVictimArray ServerPacketWriter_Test::ReserveVictims(FBuffData* Buff, uint16 NumVictim)
{
	uint64* VictimArrayBegin = Writer.Reserve<uint64>(NumVictim);
	Buff->NumVictims = NumVictim;
	Buff->VictimsOffset = reinterpret_cast<uint64>(VictimArrayBegin) - reinterpret_cast<uint64>(Packet);
	return FBuffDataVictimArray(VictimArrayBegin, NumVictim);
}

shared_ptr<FSendBuffer> ServerPacketWriter_Test::Close()
{
	// 최종 패킷 사이즈 크기
	Packet->PacketSize = Writer.GetWriteSize();
	SendBuffer->Close(Writer.GetWriteSize());
	return SendBuffer;
}
