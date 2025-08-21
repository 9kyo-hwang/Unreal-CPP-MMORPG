#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

FListener::~FListener()
{
	for (FAcceptEvent* Event : AcceptEvents)
		delete(Event);
	
	AcceptEvents.clear();
}

bool FListener::StartAccept(FServerServiceRef InService)
{
	Service = InService;
	if (Service == nullptr)
		return false;

	if (Socket.GetSocket() == INVALID_SOCKET)
		return false;

	if (Service->GetEventQueue()->Register(AsShared()) == false)
		return false;

	if (Socket.SetReuseAddress(true) == false)
		return false;

	if (Socket.SetLinger(0, 0) == false)
		return false;

	if (Socket.Bind(Service->GetNetAddress()) == false)
		return false;

	if (Socket.Listen() == false)
		return false;

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
	Socket.Close();
}

HANDLE FListener::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket.GetSocket());
}

void FListener::Dispatch(FSocketIOEvent* InEvent, int32 NumOfBytes)
{
	check(InEvent->EventType == EEventType::Accept);
	FAcceptEvent* Event = static_cast<FAcceptEvent*>(InEvent);
	ProcessAccept(Event);
}

void FListener::RegisterAccept(FAcceptEvent* InEvent)
{
	FSessionRef Session = Service->CreateSession();

	InEvent->Init();
	InEvent->Session = Session;

	DWORD BytesRecvd = 0;
	if (false == FSocketUtils::AcceptEx(Socket.GetSocket(), Session->GetSocket(), Session->RecvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & BytesRecvd, static_cast<LPOVERLAPPED>(InEvent)))
	{
		const int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			RegisterAccept(InEvent);
		}
	}
}

void FListener::ProcessAccept(FAcceptEvent* InEvent)
{
	FSessionRef Session = InEvent->Session;

	if (false == Session->SetUpdateAcceptSocket(Socket.GetSocket()))
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

	Session->SetNetAddress(FNetAddress(SockAddr));
	Session->ProcessConnect();
	RegisterAccept(InEvent);
}