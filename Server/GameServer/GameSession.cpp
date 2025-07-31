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
	ServerPacketHandler::HandlePacket(Buffer, Length);
}

void FGameSession::OnSend(int32 BytesSent)
{

}
