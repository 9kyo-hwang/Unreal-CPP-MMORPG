#include "pch.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"
#include "Session.h"

FIncomingPacketSignature GPacketHandlers[UINT16_MAX];

bool Handle_INVALID(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
{
	FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
	// TODO: Log
	return true;
}

bool Handle_S_TEST(shared_ptr<FPacketSession>& Session, Protocol::S_TEST& Packet)
{
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

	return true;
}

bool Handle_S_LOGIN(shared_ptr<FPacketSession>& Session, Protocol::S_LOGIN& Packet)
{
	return true;
}