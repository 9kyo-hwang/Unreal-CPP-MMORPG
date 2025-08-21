#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

FPacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(FPacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(FPacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	return true;
}

bool Handle_S_ENTER_GAME(FPacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	return true;
}

bool Handle_S_CHAT(FPacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	return true;
}
