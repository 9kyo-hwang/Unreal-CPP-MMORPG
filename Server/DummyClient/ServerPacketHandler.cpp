#include "pch.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"
#include "Session.h"

FIncomingPacketSignature GPacketHandlers[UINT16_MAX];

bool Handle_INVALID(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
{
	FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
	// TODO: Log
	return true;
}

bool Handle_S_LOGIN(shared_ptr<FPacketSession>& Session, Protocol::S_LOGIN& InPacket)
{
	// 3.
	if (InPacket.success() == false)
	{
		// 로그인 실패, 경고창 띄우기 등등..
		return false;
	}

	if (InPacket.players_size() == 0)
	{
		// 내 캐릭터가 없음 -> 새로 생성하라
		// 우리는 2개를 만들어뒀기 때문에 pass
	}

	// 캐릭터 선택 후 입장 UI 버튼을 눌러서 입장
	Protocol::C_ENTER Packet;
	Packet.set_playerindex(0);
	auto SendBuffer = ServerPacketHandler::CreateSendBuffer(Packet);
	Session->Send(SendBuffer);

	return true;
}

bool Handle_S_ENTER(shared_ptr<FPacketSession>& Session, Protocol::S_ENTER& InPacket)
{
	// TODO
	return true;
}

bool Handle_S_CHAT(shared_ptr<FPacketSession>& Session, Protocol::S_CHAT& InPacket)
{
	printf("%s\n", InPacket.msg().c_str());
	return true;
}
