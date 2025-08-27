#include "ClientPacketHandler.h"

#include "PacketSession.h"
#include "S1GameInstance.h"

FPacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
{
	return false;
}

bool Handle_S_LOGIN(FPacketSessionRef& Session, Protocol::S_LOGIN& InPacket)
{
	for (auto& Player : InPacket.players())
	{
		// TODO
	}

	// Lobby에서 캐릭터 선택 후 해당 인덱스 전송(0번 강제)
	Protocol::C_ENTER_GAME Packet;
	Packet.set_playerindex(0);
	Cast<US1GameInstance>(GWorld->GetGameInstance())->SendPacket(ClientPacketHandler::MakeSendBuffer(Packet));

	return true;
}

bool Handle_S_ENTER_GAME(FPacketSessionRef& Session, Protocol::S_ENTER_GAME& InPacket)
{
	return true;
}

bool Handle_S_LEAVE_GAME(FPacketSessionRef& InSession, Protocol::S_LEAVE_GAME& InPacket)
{
	return true;
}

bool Handle_S_SPAWN(FPacketSessionRef& InSession, Protocol::S_SPAWN& InPacket)
{
	return true;
}

bool Handle_S_DESPAWN(FPacketSessionRef& InSession, Protocol::S_DESPAWN& InPacket)
{
	return true;
}

bool Handle_S_CHAT(FPacketSessionRef& Session, Protocol::S_CHAT& InPacket)
{
	FString Msg = InPacket.msg().c_str();
	return true;
}
