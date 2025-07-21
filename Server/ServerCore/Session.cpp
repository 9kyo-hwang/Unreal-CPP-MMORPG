#include "pch.h"
#include "Session.h"

#include "Sockets.h"
#include "SocketSubsystem.h"

FSession::FSession()
	: RecvBuf{}
{
	Socket = FSocketSubsystem::CreateSocket();
}

FSession::~FSession()
{
	Socket->Close();
}

HANDLE FSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket->GetNativeSocket());
}

// TODO: IOCP Event에서 Recv/Send 등의 이벤트를 생성하면 이를 처리할 메서드
void FSession::Dispatch(FSocketEvent* Event, int32 NumBytes)
{
	
}
