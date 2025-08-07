#include "pch.h"
#include "ClientPacketHandler.h"

#include "ClientSession.h"
#include "GameMode.h"
#include "Player.h"

FIncomingPacketSignature GPacketHandlers[UINT16_MAX];

bool Handle_INVALID(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
{
	FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
	// TODO: Log
	return true;
}

bool Handle_C_LOGIN(shared_ptr<FPacketSession>& Session, Protocol::C_LOGIN& InPacket)
{
	// 2.
	auto ClientSession = StaticCastSharedPtr<FClientSession>(Session);

	// TODO: Validation Check(이미 로그인 된 상황에서 해커가 또 로그인 요청을 할 수도 있음 -> 상태 정보 필요)

	Protocol::S_LOGIN Packet;
	Packet.set_success(true);

	// Get Player Data from DB && Save Data to GameSession
	// 보통 세션이 아닌 계정과 관련된 클래스에 저장

	// In-game ID 발급
	static TAtomic<uint64> IdGenerator = 1;
	{
		// DB에서 가져온 플레이어 수 만큼 생성
		auto PacketPlayer = Packet.add_players();
		PacketPlayer->set_name("Player_01");
		PacketPlayer->set_type(Protocol::EPlayerType::Knight);

		// 패킷 뿐만 아니라 메모리에도 해당 정보를 들고 있어야 함
		shared_ptr<UPlayer> Player = MakeShared<UPlayer>();
		Player->PlayerId = IdGenerator++;
		Player->Name = PacketPlayer->name();
		Player->Type = PacketPlayer->type();
		Player->OwnerSession = ClientSession;

		ClientSession->Players.push_back(Player);
	}
	{
		// DB에서 가져온 플레이어 수 만큼 생성
		auto PacketPlayer = Packet.add_players();
		PacketPlayer->set_name("Player_02");
		PacketPlayer->set_type(Protocol::EPlayerType::Mage);

		// 패킷 뿐만 아니라 메모리에도 해당 정보를 들고 있어야 함
		shared_ptr<UPlayer> Player = MakeShared<UPlayer>();
		Player->PlayerId = IdGenerator++;
		Player->Name = PacketPlayer->name();
		Player->Type = PacketPlayer->type();
		Player->OwnerSession = ClientSession;

		ClientSession->Players.push_back(Player);
	}

	auto SendBuffer = ClientPacketHandler::CreateSendBuffer(Packet);
	Session->Send(SendBuffer);

	return true;
}

bool Handle_C_ENTER(shared_ptr<FPacketSession>& Session, Protocol::C_ENTER& InPacket)
{
	// 4.
	auto ClientSession = StaticCastSharedPtr<FClientSession>(Session);
	uint64 Index = InPacket.playerindex();
	// TODO: Validation

	// thread-safe -> readonly &&Players를 건드리는 건 Handle_C_LOGIN 밖에 없기 때문
	auto Player = ClientSession->Players[Index];
	ClientSession->CurrentPlayer = Player;
	ClientSession->BelongTo = GGameMode;

	// GGameMode->Add(&AGameModeBase::Login, ClientSession->CurrentPlayer);
	GGameMode->Add([GameMode = GGameMode, Player]()
		{
			GameMode->Login(Player);
		});

	Protocol::S_ENTER Packet;
	Packet.set_success(true);
	auto SendBuffer = ClientPacketHandler::CreateSendBuffer(Packet);
	ClientSession->CurrentPlayer->OwnerSession->Send(SendBuffer);

	return true;
}

bool Handle_C_CHAT(shared_ptr<FPacketSession>& Session, Protocol::C_CHAT& InPacket)
{
	printf("%s\n", InPacket.msg().c_str());

	Protocol::S_CHAT Packet;
	Packet.set_msg(InPacket.msg());

	auto SendBuffer = ClientPacketHandler::CreateSendBuffer(Packet);
	Session->Send(SendBuffer);

	// GGameMode->Add(&AGameModeBase::Broadcast, SendBuffer);
	GGameMode->Add([GameMode = GGameMode, SendBuffer]()
		{
			GameMode->Broadcast(SendBuffer);
		});

	return true;
}
