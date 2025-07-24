#include "pch.h"
#include "Session.h"

#include "Service.h"
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

void FSession::Send(BYTE* Buffer, int32 Length)
{
	// TEMP
	FSocketSend* SendEvent = New<FSocketSend>();
	SendEvent->Owner = AsShared();	// NumRefs += 1
	SendEvent->Buffer.resize(Length);
	::memcpy(SendEvent->Buffer.data(), Buffer, Length);

	WRITE_LOCK;
	RegisterSend(SendEvent);
}

void FSession::Disconnect(const TCHAR* Msg)
{
	// false로 바꾸고 기존 값을 반환받았는데, 기존 값이 이미 false라면 추가로 처리할 게 없음
	if (bIsConnected.exchange(false) == false)
	{
		return;
	}

	// 여기로 왔다는 건 bIsConnected가 true였다는 뜻
	wcout << "Disconnect: " << Msg << endl;
	OnDisconnected();	// 컨텐츠단에서 오버로딩해서 사용

	Socket->Close();
	GetService()->RemoveSession(GetSession());
}

HANDLE FSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket->GetNativeSocket());
}

// TODO: IOCP Event에서 Recv/Send 등의 이벤트를 생성하면 이를 처리할 메서드
void FSession::Dispatch(FSocketEvent* Event, int32 NumOfBytes)
{
	switch (Event->Type)
	{
	case ESocketEventTypes::Connect:
		ProcessConnect();
		break;
	case ESocketEventTypes::Accept:
		break;
	case ESocketEventTypes::Recv:
		ProcessRecv(NumOfBytes);
		break;
	case ESocketEventTypes::Send:
		ProcessSend(static_cast<FSocketSend*>(Event), NumOfBytes);
		break;
	}
}

void FSession::RegisterConnect()
{
}

void FSession::RegisterRecv()
{
	if (!IsConnected())
	{
		// 연결이 끊겼거나, 클라이언트를 강제로 접속 종료 시켰거나 등...
		return;
	}

	RecvEvent.Init();
	RecvEvent.Owner = AsShared();	// NumRefs += 1

	WSABUF Buf(countof(RecvBuf), reinterpret_cast<char*>(RecvBuf));
	DWORD NumberOfBytesRecvd = 0;
	DWORD Flags = 0;
	if (SOCKET_ERROR == ::WSARecv(Socket->GetNativeSocket(), &Buf, 1, &NumberOfBytesRecvd, &Flags, &RecvEvent, nullptr))
	{
		int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			HandleError(Error);  // 진짜 에러가 발생한 거라 반드시 오너를 해제해 참조 횟수를 줄여야 함
			RecvEvent.Owner = nullptr;	// NumRefs -= 1
		}
	}
}

void FSession::RegisterSend(FSocketSend* SendEvent)
{
	if (!IsConnected())
	{
		return;
	}

	WSABUF Buffer(SendEvent->Buffer.size(), reinterpret_cast<char*>(SendEvent->Buffer.data()));
	DWORD NumberOfBytesSent = 0;
	if (SOCKET_ERROR == ::WSASend(Socket->GetNativeSocket(), &Buffer, 1, &NumberOfBytesSent, 0, SendEvent, nullptr))
	{
		int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			HandleError(Error);
			SendEvent->Owner = nullptr;	// NumRefs -= 1
			Delete(SendEvent);
		}
	}
}

void FSession::ProcessConnect()
{
	bIsConnected.store(true);

	// Session 등록
	GetService()->AddSession(GetSession());

	// 컨텐츠 코드에서 오버로딩
	OnConnected();

	// Receive 등록(반드시 호출해야 이벤트를 탐지할 수 있음)
	RegisterRecv();
}

void FSession::ProcessRecv(int32 BytesRecvd)
{
	// 여기에 진입했다는 건 WSARecv가 성공했다는 뜻
	RecvEvent.Owner = nullptr;	// NumRefs -= 1

	if (BytesRecvd == 0)
	{
		// 연결 끊김
		Disconnect(TEXT("Recv 0"));
		return;
	}

	// 컨텐츠 코드에서 오버로딩
	OnRecv(RecvBuf, BytesRecvd);

	// 다시 이벤트를 받을 준비
	RegisterRecv();
}

void FSession::ProcessSend(FSocketSend* SendEvent, int32 BytesSent)
{
	SendEvent->Owner = nullptr;	// NumRefs -= 1
	Delete(SendEvent);	// SendEvent는 더 이상 사용하지 않음

	if (BytesSent == 0)
	{
		Disconnect(TEXT("Send 0"));
		return;
	}

	// 컨텐츠 코드에서 오버로딩(딱히 할 일은 없을 것)
	OnSend(BytesSent);
}

void FSession::HandleError(int32 Error)
{
	switch (Error)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;

	default:
		// TODO: Log
		printf("Handle Error: %d\n", Error);
		break;
	}
}
