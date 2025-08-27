#include "ClientPacketHandler.h"

FPacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
{
	return false;
}

bool Handle_S_LOGIN(FPacketSessionRef& Session, Protocol::S_LOGIN& Packet)
{
	return true;
}

bool Handle_S_ENTER_GAME(FPacketSessionRef& Session, Protocol::S_ENTER_GAME& Packet)
{
	return true;
}

bool Handle_S_LEAVE_GAME(FPacketSessionRef& InSession, Protocol::S_LEAVE_GAME& Packet)
{
	return true;
}

bool Handle_S_SPAWN(FPacketSessionRef& InSession, Protocol::S_SPAWN& Packet)
{
	return true;
}

bool Handle_S_DESPAWN(FPacketSessionRef& InSession, Protocol::S_DESPAWN& Packet)
{
	return true;
}

bool Handle_S_CHAT(FPacketSessionRef& Session, Protocol::S_CHAT& Packet)
{
	FString Msg = Packet.msg().c_str();
	return true;
}
