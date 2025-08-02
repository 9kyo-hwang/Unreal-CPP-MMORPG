#include "pch.h"
#include "GameSession.h"

#include "ServerPacketHandler.h"
#include "SessionManager.h"

void FGameSession::OnConnected()
{
	GSessionManager.Add(SharedThis(this));
}

void FGameSession::OnDisconnected()
{
	GSessionManager.Remove(SharedThis(this));
}

void FGameSession::OnReceive(BYTE* Buffer, int32 Length)
{
	// 여기에 진입했다는 것은 온전한 패킷이 보장됨
	shared_ptr<FPacketSession> Session = shared_ptr<FPacketSession>();

	// 추후 서버가 여러 용도로 분산되어 있다면, ID 대역폭을 보고 적절한 핸들러를 적용해야 함
	FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);

	// TODO: Packet Id 대역 확인
	ServerPacketHandler::Incoming(Session, Buffer, Length);
}

void FGameSession::OnSend(int32 BytesSent)
{

}
