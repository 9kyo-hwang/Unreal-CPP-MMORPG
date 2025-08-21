#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

/*--------------
	FSession
---------------*/

FSession::FSession() : RecvBuffer(BUFFER_SIZE)
{
	Socket = SocketUtils::CreateSocket();
}

FSession::~FSession()
{
	SocketUtils::Close(Socket);
}

void FSession::Send(FSendBufferRef InSendBuffer)
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;

	// 현재 RegisterSend가 걸리지 않은 상태라면, 걸어준다
	{
		FScopeLock ScopeLock(CriticalSection);

		SendQueue.push(InSendBuffer);

		if (bIsSendRegistered.exchange(true) == false)
			registerSend = true;
	}
	
	if (registerSend)
		RegisterSend();
}

bool FSession::Connect()
{
	return RegisterConnect();
}

void FSession::Disconnect(const WCHAR* Msg)
{
	if (bIsConnected.exchange(false) == false)
		return;

	// TEMP
	wcout << "Disconnect : " << Msg << endl;

	RegisterDisconnect();
}

HANDLE FSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket);
}

void FSession::Dispatch(FSocketIOEvent* InEvent, int32 NumOfBytes)
{
	switch (InEvent->EventType)
	{
	case EEventType::Connect:
		ProcessConnect();
		break;
	case EEventType::Disconnect:
		ProcessDisconnect();
		break;
	case EEventType::Recv:
		ProcessRecv(NumOfBytes);
		break;
	case EEventType::Send:
		ProcessSend(NumOfBytes);
		break;
	default:
		break;
	}
}

bool FSession::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != EServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(Socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(Socket, 0/*남는거*/) == false)
		return false;

	ConnectEvent.Init();
	ConnectEvent.Owner = AsShared(); // ADD_REF

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketUtils::ConnectEx(Socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &ConnectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			ConnectEvent.Owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

bool FSession::RegisterDisconnect()
{
	DisconnectEvent.Init();
	DisconnectEvent.Owner = AsShared(); // ADD_REF

	if (false == SocketUtils::DisconnectEx(Socket, &DisconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			DisconnectEvent.Owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

void FSession::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	RecvEvent.Init();
	RecvEvent.Owner = AsShared(); // ADD_REF

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(RecvBuffer.WritePos());
	wsaBuf.len = RecvBuffer.GetFreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(Socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &RecvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			RecvEvent.Owner = nullptr; // RELEASE_REF
		}
	}
}

void FSession::RegisterSend()
{
	if (IsConnected() == false)
		return;

	SendEvent.Init();
	SendEvent.Owner = AsShared(); // ADD_REF

	// 보낼 데이터를 sendEvent에 등록
	{
		FScopeLock ScopeLock(CriticalSection);

		int32 writeSize = 0;
		while (SendQueue.empty() == false)
		{
			FSendBufferRef sendBuffer = SendQueue.front();

			writeSize += sendBuffer->GetWriteSize();
			// TODO : 예외 체크

			SendQueue.pop();
			SendEvent.SendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather (흩어져 있는 데이터들을 모아서 한 방에 보낸다)
	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(SendEvent.SendBuffers.size());
	for (FSendBufferRef sendBuffer : SendEvent.SendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->GetData());
		wsaBuf.len = static_cast<LONG>(sendBuffer->GetWriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(Socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT &numOfBytes, 0, &SendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			SendEvent.Owner = nullptr; // RELEASE_REF
			SendEvent.SendBuffers.clear(); // RELEASE_REF
			bIsSendRegistered.store(false);
		}
	}
}

void FSession::ProcessConnect()
{
	ConnectEvent.Owner = nullptr; // RELEASE_REF

	bIsConnected.store(true);

	// 세션 등록
	GetService()->AddSession(GetSessionRef());

	// 컨텐츠 코드에서 재정의
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void FSession::ProcessDisconnect()
{
	DisconnectEvent.Owner = nullptr; // RELEASE_REF

	OnDisconnected(); // 컨텐츠 코드에서 재정의
	GetService()->ReleaseSession(GetSessionRef());
}

void FSession::ProcessRecv(int32 BytesRecvd)
{
	RecvEvent.Owner = nullptr; // RELEASE_REF

	if (BytesRecvd == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (RecvBuffer.OnWrite(BytesRecvd) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = RecvBuffer.GetDataSize();
	int32 processLen = OnRecv(RecvBuffer.ReadPos(), dataSize); // 컨텐츠 코드에서 재정의
	if (processLen < 0 || dataSize < processLen || RecvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}
	
	// 커서 정리
	RecvBuffer.Clear();

	// 수신 등록
	RegisterRecv();
}

void FSession::ProcessSend(int32 BytesSent)
{
	SendEvent.Owner = nullptr; // RELEASE_REF
	SendEvent.SendBuffers.clear(); // RELEASE_REF

	if (BytesSent == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 코드에서 재정의
	OnSend(BytesSent);

	FScopeLock ScopeLock(CriticalSection);
	if (SendQueue.empty())
		bIsSendRegistered.store(false);
	else
		RegisterSend();
}

void FSession::HandleError(int32 ErrorCode)
{
	switch (ErrorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		cout << "Handle Error : " << ErrorCode << endl;
		break;
	}
}

/*-----------------
	FPacketSession
------------------*/

FPacketSession::FPacketSession()
{
}

FPacketSession::~FPacketSession()
{
}

// [size(2)][id(2)][data....][size(2)][id(2)][data....]
int32 FPacketSession::OnRecv(BYTE* InBuffer, int32 InLength)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = InLength - processLen;
		// 최소한 헤더는 파싱할 수 있어야 한다
		if (dataSize < sizeof(FPacketHeader))
			break;

		FPacketHeader header = *(reinterpret_cast<FPacketHeader*>(&InBuffer[processLen]));
		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다
		if (dataSize < header.Size)
			break;

		// 패킷 조립 성공
		OnReceive(&InBuffer[processLen], header.Size);

		processLen += header.Size;
	}

	return processLen;
}
