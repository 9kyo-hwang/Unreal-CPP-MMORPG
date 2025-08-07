#include "pch.h"
#include "ClientSession.h"

#include "ClientPacketHandler.h"
#include "GameMode.h"
#include "SessionManager.h"

FClientSession::FClientSession()
{
}

FClientSession::~FClientSession()
{
	printf("FClientSession::~FClientSession()\n");
}

void FClientSession::OnConnected()
{
	GSessionManager.Add(SharedThis(this));
}

void FClientSession::OnDisconnected()
{
	GSessionManager.Remove(SharedThis(this));

	if (CurrentPlayer)
	{
		if (auto GameMode = BelongTo.lock())
		{
			// GameMode->Add(&AGameModeBase::Logout, CurrentPlayer);
			GameMode->Add([Player = CurrentPlayer, GameMode]()
				{
					GameMode->Logout(Player);
				});
		}
	}

	CurrentPlayer = nullptr;
	Players.clear();
}

void FClientSession::OnReceive(BYTE* Buffer, int32 Length)
{
	// 여기에 진입했다는 것은 온전한 패킷이 보장됨
	shared_ptr<FPacketSession> Session = SharedThisSession();

	// 추후 서버가 여러 용도로 분산되어 있다면, ID 대역폭을 보고 적절한 핸들러를 적용해야 함
	FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);

	// TODO: Packet Id 대역 확인
	ClientPacketHandler::Incoming(Session, Buffer, Length);
}

void FClientSession::OnSend(int32 BytesSent)
{

}
