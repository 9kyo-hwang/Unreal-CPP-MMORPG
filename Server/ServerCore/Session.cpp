#include "pch.h"
#include "Session.h"

#include "Service.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

FSession::FSession()
	: RecvBuffer(BufferSize)
{
	Socket = FSocketSubsystem::CreateSocket();
}

FSession::~FSession()
{
	Socket->Close();
}

void FSession::Send(shared_ptr<FSendBuffer> SendBuffer)
{
	/**
	 *	현재 예약된 Send 이벤트가 없다면 전송 이벤트 예약
	 *	아니라면 Queue에 저장
	 */

	WRITE_LOCK;

	SendQueue.push(SendBuffer);	// 추후 lock-free 방식 queue로 사용할 수도 있어서 LOCK + TAtomic 사용
	if (bIsSending.exchange(true) == false)
	{
		RegisterSend();
	}
}

bool FSession::Connect()
{
	return RegisterConnect();
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

	OnDisconnected();	// 컨텐츠단에서 재정의해서 사용
	GetService()->RemoveSession(GetSession());

	RegisterDisconnect();
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
	case ESocketEventTypes::Disconnect:
		ProcessDisconnect();
		break;
	case ESocketEventTypes::Accept:
		break;
	case ESocketEventTypes::Recv:
		ProcessRecv(NumOfBytes);
		break;
	case ESocketEventTypes::Send:
		ProcessSend(NumOfBytes);
		break;
	}
}

bool FSession::RegisterConnect()
{
	if (IsConnected())
	{
		return false;
	}

	// 본인이 클라이언트가 아니라면 Connect를 등록할 수 없음
	if (GetService()->GetType() != EServiceType::Client)
	{
		return false;
	}

	if (!Socket->SetReuseAddr())
	{
		return false;
	}

	Addr.SetAnyAddress();
	Addr.SetPort(0);	// Port는 0으로 설정하면 OS가 자동으로 할당함
	if (!Socket->Bind(Addr))
	{
		return false;
	}

	ConnectEvent.Init();
	ConnectEvent.Owner = AsShared();	// NumRefs += 1

	DWORD BytesSent = 0;
	auto TargetAddr = GetService()->GetAddr().GetRawAddr();

	bool bResult = FSocketSubsystem::Connect(
		Socket->GetNativeSocket(), 
		reinterpret_cast<const SOCKADDR*>(TargetAddr), 
		sizeof(SOCKADDR_IN),
		nullptr, 
		0, 
		&BytesSent, 
		&ConnectEvent
	);

	if (bResult == false)
	{
		int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			ConnectEvent.Owner = nullptr;	// NumRefs -= 1
			return false;
		}
	}

	return true;
}

bool FSession::RegisterDisconnect()
{
	DisconnectEvent.Init();
	DisconnectEvent.Owner = AsShared();	// NumRefs += 1

	if (!FSocketSubsystem::Disconnect(
		Socket->GetNativeSocket(), 
		&DisconnectEvent, 
		TF_REUSE_SOCKET, 
		0
	))
	{
		int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			DisconnectEvent.Owner = nullptr;	// NumRefs -= 1
			return false;
		}
	}

	return true;
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

	WSABUF Buf(RecvBuffer.GetFreeSize(), reinterpret_cast<char*>(RecvBuffer.GetWritePosition()));
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

void FSession::RegisterSend()
{
	// bIsSending Atomic 변수 덕에 한 번에 한 스레드만 들어오는 것이 보장

	if (!IsConnected())
	{
		return;
	}

	SendEvent.Init();
	SendEvent.Owner = AsShared();	// NumRefs += 1

	{
		WRITE_LOCK;	// 나중에 코드가 바뀔 수도 있어서 다시 Lock을 걸어줌

		int32 WriteSize = 0;
		while (!SendQueue.empty())
		{
			auto SendBuffer = SendQueue.front();
			WriteSize += SendBuffer->Num();

			// TODO: 크기가 너무 크면 더 이상 전송하지 않도록 break

			SendQueue.pop();
			SendEvent.SendBuffers.push_back(SendBuffer);
		}
	}

	// Scatter-Gather
	DWORD BufferCount = SendEvent.SendBuffers.size();
	TArray<WSABUF> Buffers(BufferCount);
	for (int32 i = 0; i < BufferCount; ++i)
	{
		Buffers[i] = WSABUF(
			SendEvent.SendBuffers[i]->Num(),
			reinterpret_cast<char*>(SendEvent.SendBuffers[i]->GetData())
		);
	}

	DWORD NumberOfBytesSent = 0;
	if (SOCKET_ERROR == ::WSASend(Socket->GetNativeSocket(), Buffers.data(), BufferCount, &NumberOfBytesSent, 0, &SendEvent, nullptr))
	{
		int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			HandleError(Error);
			SendEvent.Owner = nullptr;	// NumRefs -= 1
			SendEvent.SendBuffers.clear();	// 전송 버퍼들을 지워서 참조 횟수를 날림
			bIsSending.store(false);
		}
	}
}

void FSession::ProcessConnect()
{
	ConnectEvent.Owner = nullptr;
	bIsConnected.store(true);

	// Session 등록
	GetService()->AddSession(GetSession());

	// 컨텐츠단에서 재정의해서 사용
	OnConnected();

	// Receive 등록(반드시 호출해야 이벤트를 탐지할 수 있음)
	RegisterRecv();
}

void FSession::ProcessDisconnect()
{
	DisconnectEvent.Owner = nullptr;	// NumRefs -= 1
}

void FSession::ProcessRecv(int32 BytesToRecv)
{
	// 여기에 진입했다는 건 WSARecv가 성공했다는 뜻
	RecvEvent.Owner = nullptr;	// NumRefs -= 1

	if (BytesToRecv == 0)
	{
		// 연결 끊김
		Disconnect(TEXT("Recv 0"));
		return;
	}

	if (!RecvBuffer.AdvanceWritePosition(BytesToRecv))
	{
		Disconnect(TEXT("RecvBuffer Overflow: AdvanceWritePosition"));
		return;
	}

	// 실제 데이터 시작 위치부터 누적된 데이터 크기만큼
	int32 DataSize = RecvBuffer.GetDataSize();

	// 해당 함수에서 반환하는 길이값은 "실제로 처리한 데이터 길이"
	int32 BytesProcess = OnRecv(RecvBuffer.GetReadPosition(), DataSize);

	// 따라서 처리한 데이터 길이만큼 다시 수신 버퍼의 커서를 옮겨줘야 함
	if (BytesProcess < 0 || BytesProcess > DataSize || !RecvBuffer.AdvanceReadPosition(BytesProcess))
	{
		Disconnect(TEXT("RecvBuffer Overflow: AdvanceReadPosition"));
		return;
	}

	RecvBuffer.Clear();

	// 다시 이벤트를 받을 준비
	RegisterRecv();
}

void FSession::ProcessSend(int32 BytesToSend)
{
	SendEvent.Owner = nullptr;	// NumRefs -= 1
	SendEvent.SendBuffers.clear();  // 전송 버퍼들을 지워서 참조 횟수를 날림

	if (BytesToSend == 0)
	{
		Disconnect(TEXT("Send 0"));
		return;
	}

	// 컨텐츠단에서 재정의해서 사용(딱히 할 일은 없을 것)
	OnSend(BytesToSend);

	WRITE_LOCK;
	if (SendQueue.empty())
	{
		bIsSending.store(false);
	}
	else
	{
		// 큐가 비어있지 않다는 건 다른 스레드에서 Send를 호출해 데이터를 추가한 것
		RegisterSend();
	}
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
