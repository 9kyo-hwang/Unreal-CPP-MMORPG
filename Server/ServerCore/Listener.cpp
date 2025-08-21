#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

/*--------------
	FListener
---------------*/

FListener::~FListener()
{
	SocketUtils::Close(Socket);

	for (FAcceptEvent* Event : AcceptEvents)
	{
		// TODO

		delete(Event);
	}
}

bool FListener::StartAccept(FServerServiceRef InService)
{
	Service = InService;
	if (Service == nullptr)
	{
		return false;
	}

	Socket = SocketUtils::CreateSocket();
	if (Socket == INVALID_SOCKET)
	{
		return false;
	}

	if (Service->GetEventQueue()->Register(AsShared()) == false)
	{
		return false;
	}

	if (SocketUtils::SetReuseAddress(Socket, true) == false)
	{
		return false;
	}

	if (SocketUtils::SetLinger(Socket, 0, 0) == false)
	{
		return false;
	}

	if (SocketUtils::Bind(Socket, Service->GetNetAddress()) == false)
	{
		return false;
	}

	if (SocketUtils::Listen(Socket) == false)
	{
		return false;
	}

	const int32 AcceptCount = Service->GetMaxSessionCount();
	for (int32 i = 0; i < AcceptCount; i++)
	{
		FAcceptEvent* Event = new FAcceptEvent();
		Event->Owner = AsShared();
		AcceptEvents.push_back(Event);
		RegisterAccept(Event);
	}

	return true;
}

void FListener::CloseSocket()
{
	SocketUtils::Close(Socket);
}

HANDLE FListener::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket);
}

void FListener::Dispatch(FSocketIOEvent* InEvent, int32 NumOfBytes)
{
	check(InEvent->EventType == EEventType::Accept);
	FAcceptEvent* Event = static_cast<FAcceptEvent*>(InEvent);
	ProcessAccept(Event);
}

void FListener::RegisterAccept(FAcceptEvent* InEvent)
{
	FSessionRef Session = Service->CreateSession(); // Register IOCP

	InEvent->Init();
	InEvent->Session = Session;

	DWORD BytesRecvd = 0;
	if (false == SocketUtils::AcceptEx(Socket, Session->GetSocket(), Session->RecvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & BytesRecvd, static_cast<LPOVERLAPPED>(InEvent)))
	{
		const int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			// 일단 다시 Accept 걸어준다
			RegisterAccept(InEvent);
		}
	}
}

void FListener::ProcessAccept(FAcceptEvent* InEvent)
{
	FSessionRef Session = InEvent->Session;

	if (false == SocketUtils::SetUpdateAcceptSocket(Session->GetSocket(), Socket))
	{
		RegisterAccept(InEvent);
		return;
	}

	SOCKADDR_IN SockAddr;
	int32 AddrLen = sizeof(SockAddr);
	if (SOCKET_ERROR == ::getpeername(Session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&SockAddr), &AddrLen))
	{
		RegisterAccept(InEvent);
		return;
	}

	Session->SetNetAddress(NetAddress(SockAddr));
	Session->ProcessConnect();
	RegisterAccept(InEvent);
}