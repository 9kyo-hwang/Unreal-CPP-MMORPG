#include "pch.h"
#include "ClientPacketHandler.h"

#include "BufferReader.h"
#include "Protocol.pb.h"
#include "Session.h"

void ClientPacketHandler::HandlePacket(BYTE* Buffer, int32 Length)
{
	FBufferReader br(Buffer, Length);
	FPacketHeader PacketHeader; br >> PacketHeader;

	switch (PacketHeader.Id)
	{
	case EPacketFromServer::Test:
		HandleTestPacket(Buffer, Length);
		break;
	default:
		break;
	}
}

void ClientPacketHandler::HandleTestPacket(BYTE* Buffer, int32 Length)
{
	// Server에서 Serialize한 것을 여기서 Deserialize
	Protocol::S_TEST Packet;

	// Packet은 데이터를 담는 영역이므로, 시작 주소는 Buffer + 헤더 크기, 데이터 크기는 전체 길이 - 헤더 크기
	uint16 PacketHeaderSize = sizeof(FPacketHeader);
	check(Packet.ParseFromArray(Buffer + PacketHeaderSize, Length - PacketHeaderSize));

	printf("Packet ID: %llu, Hp: %d, Attack: %u\n", Packet.id(), Packet.hp(), Packet.attack());
	printf("Buffs Length: %d\n", Packet.buffs_size());

	for (auto& Buff : Packet.buffs())
	{
		printf("  BuffData: [%llu, %f]\n", Buff.buffid(), Buff.remaintime());
		printf("  Victims Length: %d\n", Buff.victims_size());
		for (auto& Victim : Buff.victims())
		{
			printf("    Victim: %llu\n", Victim);
		}
	}

	printf("\n");
}
