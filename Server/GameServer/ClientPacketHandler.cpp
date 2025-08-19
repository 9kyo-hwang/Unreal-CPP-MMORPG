#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"

FIncomingPacketSignature GPacketHandlers[UINT16_MAX];

bool Handle_INVALID(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
{
	FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
	// TODO: Log
	return true;
}

bool Handle_C_LOGIN(shared_ptr<FPacketSession>& Session, Protocol::C_LOGIN& InPacket)
{
	return true;
}

bool Handle_C_ENTER(shared_ptr<FPacketSession>& Session, Protocol::C_ENTER& InPacket)
{
	return true;
}

bool Handle_C_CHAT(shared_ptr<FPacketSession>& Session, Protocol::C_CHAT& InPacket)
{
	return true;
}
