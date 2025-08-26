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

bool Handle_S_CHAT(FPacketSessionRef& Session, Protocol::S_CHAT& Packet)
{
	FString Msg = Packet.msg().c_str();
	return true;
}
