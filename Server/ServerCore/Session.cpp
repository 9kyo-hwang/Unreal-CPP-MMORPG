#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

FSession::FSession() : RecvBuffer(BUFFER_SIZE)
{
}

FSession::~FSession()
{
}

void FSession::Send(FSendBufferRef InSendBuffer)
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;
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

	wcout << L"Disconnect : " << Msg << endl;

	RegisterDisconnect();
}

HANDLE FSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket.GetSocket());
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

	if (Socket.SetReuseAddress(true) == false)
		return false;

	if (Socket.Bind(FNetAddress(L"127.0.0.1", 0)) == false)
		return false;

	ConnectEvent.Init();
	ConnectEvent.Owner = AsShared();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == FSocketUtils::ConnectEx(Socket.GetSocket(), reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &ConnectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			ConnectEvent.Owner = nullptr;
			return false;
		}
	}

	return true;
}

bool FSession::RegisterDisconnect()
{
	DisconnectEvent.Init();
	DisconnectEvent.Owner = AsShared();

	if (false == FSocketUtils::DisconnectEx(Socket.GetSocket(), &DisconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			DisconnectEvent.Owner = nullptr;
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
	RecvEvent.Owner = AsShared();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(RecvBuffer.WritePos());
	wsaBuf.len = RecvBuffer.GetFreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(Socket.GetSocket(), &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &RecvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			RecvEvent.Owner = nullptr;
		}
	}
}

void FSession::RegisterSend()
{
	if (IsConnected() == false)
		return;

	SendEvent.Init();
	SendEvent.Owner = AsShared();
	{
		FScopeLock ScopeLock(CriticalSection);

		int32 writeSize = 0;
		while (SendQueue.empty() == false)
		{
			FSendBufferRef sendBuffer = SendQueue.front();

			writeSize += sendBuffer->GetWriteSize();

			SendQueue.pop();
			SendEvent.SendBuffers.push_back(sendBuffer);
		}
	}

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
	if (SOCKET_ERROR == ::WSASend(Socket.GetSocket(), wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT &numOfBytes, 0, &SendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			SendEvent.Owner = nullptr;
			SendEvent.SendBuffers.clear();
			bIsSendRegistered.store(false);
		}
	}
}

void FSession::ProcessConnect()
{
	ConnectEvent.Owner = nullptr;

	bIsConnected.store(true);

	GetService()->AddSession(GetSessionRef());

	OnConnected();

	RegisterRecv();
}

void FSession::ProcessDisconnect()
{
	DisconnectEvent.Owner = nullptr;

	OnDisconnected();
	GetService()->ReleaseSession(GetSessionRef());
}

void FSession::ProcessRecv(int32 BytesRecvd)
{
	RecvEvent.Owner = nullptr;

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
	int32 processLen = OnRecv(RecvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || RecvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}
	
	RecvBuffer.Clear();

	RegisterRecv();
}

void FSession::ProcessSend(int32 BytesSent)
{
	SendEvent.Owner = nullptr;
	SendEvent.SendBuffers.clear();

	if (BytesSent == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

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
		cout << "Handle Error : " << ErrorCode << endl;
		break;
	}
}

FPacketSession::FPacketSession()
{
}

FPacketSession::~FPacketSession()
{
}

int32 FPacketSession::OnRecv(BYTE* InBuffer, int32 InLength)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = InLength - processLen;
		if (dataSize < sizeof(FPacketHeader))
			break;

		FPacketHeader header = *(reinterpret_cast<FPacketHeader*>(&InBuffer[processLen]));
		if (dataSize < header.Size)
			break;

		OnReceive(&InBuffer[processLen], header.Size);

		processLen += header.Size;
	}

	return processLen;
}
