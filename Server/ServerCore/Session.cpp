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
	{
		return;
	}

	FScopeLock ScopeLock(CriticalSection);

	SendQueue.push(InSendBuffer);
	if (bIsRegistering.exchange(true) == false)
	{
		RegisterSend();
	}
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

	DWORD BytesSent = 0;
	SOCKADDR_IN SockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == FSocketUtils::ConnectEx(
		Socket.GetSocket(), 
		reinterpret_cast<SOCKADDR*>(&SockAddr), 
		sizeof(SockAddr), 
		nullptr, 
		0, 
		&BytesSent, 
		&ConnectEvent)
		)
	{
		int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
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

	WSABUF Buffer
	{
		static_cast<ULONG>(RecvBuffer.GetFreeSize()),
		reinterpret_cast<char*>(RecvBuffer.WritePos())
	};

	DWORD BytesRecvd = 0;
	DWORD Flags = 0;
	if (SOCKET_ERROR == ::WSARecv(Socket.GetSocket(), &Buffer, 1, OUT &BytesRecvd, OUT &Flags, &RecvEvent, nullptr))
	{
		int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			HandleError(Error);
			RecvEvent.Owner = nullptr;
		}
	}
}

void FSession::RegisterSend()
{
	if (IsConnected() == false)
	{
		return;
	}

	SendEvent.Init();
	SendEvent.Owner = AsShared();

	int32 WriteSize = 0;
	while (SendQueue.empty() == false)
	{
		FSendBufferRef SendBuffer = SendQueue.front();
		WriteSize += SendBuffer->GetWriteSize();

		SendQueue.pop();
		SendEvent.SendBuffers.push_back(SendBuffer);
	}

	vector<WSABUF> Buffers;
	Buffers.reserve(SendEvent.SendBuffers.size());
	for (FSendBufferRef SendBuffer : SendEvent.SendBuffers)
	{
		WSABUF Buffer
		{
			static_cast<ULONG>(SendBuffer->GetWriteSize()),
			reinterpret_cast<CHAR*>(SendBuffer->GetData())
		};

		Buffers.emplace_back(Buffer);
	}

	DWORD BytesSent = 0;
	if (SOCKET_ERROR == ::WSASend(
		Socket.GetSocket(), 
		Buffers.data(), 
		static_cast<DWORD>(Buffers.size()), 
		OUT &BytesSent, 
		0, 
		&SendEvent, 
		nullptr)
		)
	{
		int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			HandleError(Error);
			SendEvent.Owner = nullptr;
			SendEvent.SendBuffers.clear();
			bIsRegistering.store(false);
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
	{
		bIsRegistering.store(false);
	}
	else
	{
		RegisterSend();
	}
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
	int32 ProcessedSize = 0;

	while (true)
	{
		int32 DataSize = InLength - ProcessedSize;
		if (DataSize < sizeof(PacketHeader))
			break;

		PacketHeader Header = *reinterpret_cast<PacketHeader*>(&InBuffer[ProcessedSize]);
		if (DataSize < Header.Size)
			break;

		OnReceive(&InBuffer[ProcessedSize], Header.Size);

		ProcessedSize += Header.Size;
	}

	return ProcessedSize;
}
