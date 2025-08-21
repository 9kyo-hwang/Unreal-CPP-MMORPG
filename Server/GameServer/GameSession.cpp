#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"
//#include "Room.h"

void FGameSession::OnConnected()
{
	GSessionManager.Add(SharedThis<FGameSession>(this));
}

void FGameSession::OnDisconnected()
{
	GSessionManager.Remove(SharedThis<FGameSession>(this));
}

void FGameSession::OnReceive(BYTE* buffer, int32 len)
{
	FPacketSessionRef session = GetPacketSessionRef();
	FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);

	// TODO
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void FGameSession::OnSend(int32 len)
{
}