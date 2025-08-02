#include "pch.h"
#include "ClientPacketHandler.h"

FIncomingPacketSignature GPacketHandlers[UINT16_MAX];

bool Handle_INVALID(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
{
	FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
	// TODO: Log
	return true;
}

bool Handle_C_TEST(shared_ptr<FPacketSession>& Session, Protocol::C_TEST& Packet)
{
	return true;
}
