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

int32 FGameSession::OnRecv(BYTE* Buffer, int32 Length)
{
	cout << "OnRecv Len = " << Length << endl;

	shared_ptr<FSendBuffer> SendBuffer = MakeShared<FSendBuffer>(4096);
	SendBuffer->PushData(Buffer, Length);

	GSessionManager.Broadcast(SendBuffer); // Broadcast to all sessions

	return Length;
}

void FGameSession::OnSend(int32 BytesSent)
{
	cout << "OnSend Len = " << BytesSent << endl;
}
