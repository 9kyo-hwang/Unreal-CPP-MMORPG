#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"

FPacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(FPacketSessionRef& session, BYTE* buffer, int32 len)
{
	FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);
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
