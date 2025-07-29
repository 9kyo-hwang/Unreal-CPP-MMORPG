#include "pch.h"
#include "GameSession.h"

#include "SessionManager.h"

void FGameSession::OnConnected()
{
	GSessionManager.Add(SharedThis(this));
}

void FGameSession::OnDisconnected()
{
	GSessionManager.Remove(SharedThis(this));
}

int32 FGameSession::OnReceive(BYTE* Buffer, int32 Length)
{
	// 여기에 진입했다는 것은 온전한 패킷이 보장됨
	FPacketHeader PacketHeader = *reinterpret_cast<FPacketHeader*>(Buffer);
	printf("ID: %d\tSize: %d\n", PacketHeader.Id, PacketHeader.Size);
	return Length;
}

void FGameSession::OnSend(int32 BytesSent)
{

}
