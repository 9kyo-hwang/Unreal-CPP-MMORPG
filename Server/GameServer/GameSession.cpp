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

void FGameSession::OnReceive(BYTE* Buffer, int32 Length)
{
	FPacketSessionRef Session = GetPacketSessionRef();
	FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);

	// TODO
	ServerPacketHandler::HandlePacket(Session, Buffer, Length);
}

void FGameSession::OnSend(int32 Length)
{
}