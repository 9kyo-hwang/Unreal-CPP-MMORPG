#include "pch.h"
#include "ServerPacketHandler.h"

FPacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
{
	PacketHeader* Header = reinterpret_cast<PacketHeader*>(Buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(FPacketSessionRef& Session, Protocol::C_LOGIN& Packet)
{
	return true;
}

bool Handle_C_ENTER_GAME(FPacketSessionRef& Session, Protocol::C_ENTER_GAME& Packet)
{
	return true;
}

bool Handle_C_LEAVE_GAME(FPacketSessionRef& Session, Protocol::C_LEAVE_GAME& Packet)
{
	return true;
}

bool Handle_C_CHAT(FPacketSessionRef& Session, Protocol::C_CHAT& Packet)
{
	return true;
}
