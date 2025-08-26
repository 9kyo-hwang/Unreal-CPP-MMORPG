#include "pch.h"
#include "ServerPacketHandler.h"

FPacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
{
	PacketHeader* Header = reinterpret_cast<PacketHeader*>(Buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(FPacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	return true;
}

bool Handle_C_ENTER_GAME(FPacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	return true;
}

bool Handle_C_CHAT(FPacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	return true;
}
