#include "pch.h"
#include "ServerPacketHandler.h"

#include "ActorFactory.h"
#include "GameSession.h"
#include "MathUtility.h"
#include "Player.h"
#include "World.h"

FPacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
{
	PacketHeader* Header = reinterpret_cast<PacketHeader*>(Buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(FPacketSessionRef& Session, Protocol::C_LOGIN& InPacket)
{
	// 1. 클라이언트가 Login을 요청함

	/**
	 *	TODO: DB에서 Account 정보 및 유저 정보를 가져옴
	 *	- 원래는 인증 서버(WebServer)를 먼저 거침
	 *	- 인증이 성공했다는 정보는 Redis같은 NoSQL 쪽 토큰으로 저장
	 *	- GameServer는 Redis에 접근해 해당 토큰을 보고 인증이 된 유저에 한해 접속시킴
	 */

	Protocol::S_LOGIN Packet;
	for (int32 i = 0; i < 3; ++i)	// 캐릭터가 3개 있다고 가정
	{
		Protocol::PlayerInfo* Player = Packet.add_players();
		Player->set_x(FMath::RandRange(0.f, 100.f));
		Player->set_y(FMath::RandRange(0.f, 100.f));
		Player->set_z(FMath::RandRange(0.f, 100.f));
		Player->set_yaw(FMath::RandRange(0.f, 100.f));
	}

	Packet.set_success(true);

	Session->Send(ServerPacketHandler::MakeSendBuffer(Packet));
	return true;
}

bool Handle_C_ENTER_GAME(FPacketSessionRef& Session, Protocol::C_ENTER_GAME& InPacket)
{
	// Player 생성 후 Room 입장 vs Room 안에서 Player 생성
	auto Player = ActorFactory::SpawnPlayer(static_pointer_cast<FGameSession>(Session));
	GWorld->DoAsync(&FWorld::EnterPlayer, Player);
	// GWorld->EnterPlayer(Player);

	return true;
}

bool Handle_C_LEAVE_GAME(FPacketSessionRef& Session, Protocol::C_LEAVE_GAME& InPacket)
{
	if (FGameSessionRef GameSession = StaticCastSharedPtr<FGameSession>(Session))
	{
		if (TSharedPtr<APlayer> Player = GameSession->GetPlayer())
		{
			// 플레이어가 속한 월드 정보를 가져와야 함
			if (TSharedPtr<FWorld> World = Player->GetWorld())
			{
				World->DoAsync(&FWorld::LeavePlayer, Player);
				// World->LeavePlayer(Player);
				return true;
			}
		}
	}

	return false;
}

bool Handle_C_MOVE(FPacketSessionRef& InSession, Protocol::C_MOVE& Packet)
{
	if (FGameSessionRef GameSession = StaticCastSharedPtr<FGameSession>(InSession))
	{
		if (TSharedPtr<APlayer> Player = GameSession->GetPlayer())
		{
			if (TSharedPtr<FWorld> World = Player->GetWorld())
			{
				// TODO: Validation

				World->DoAsync(&FWorld::MovePlayer, Packet);
				// World->MovePlayer(Packet);
				return true;
			}
		}
	}

	return false;
}

bool Handle_C_CHAT(FPacketSessionRef& Session, Protocol::C_CHAT& InPacket)
{
	return true;
}
