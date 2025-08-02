#include "pch.h"
#include "ServerPacketHandler.h"

FIncomingPacketSignature GPacketHandlers[UINT16_MAX];

bool Handle_INVALID(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
{
	FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
	// TODO: Log
	return true;
}

bool Handle_S_TEST(shared_ptr<FPacketSession>& Session, Protocol::S_TEST& Packet)
{
	return true;
}
