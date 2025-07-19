#include "pch.h"
#include "Listener.h"
#include "IOCPEvent.h"
#include "Session.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

FListener::FListener()
	: Socket(nullptr)
{
	
}

FListener::~FListener()
{
	Socket->Close();

	for (FOverlapped_Accept* Event : AcceptEvents)
	{
		// TODO

		Delete(Event);
	}
}

HANDLE FListener::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket->GetNativeSocket());
}

void FListener::Dispatch(FOverlapped* Event, int32 NumBytes)
{
	check(Event->GetEventType() == EIoEvent::Accept);
	FOverlapped_Accept* AcceptEvent = static_cast<FOverlapped_Accept*>(Event);
	ProcessAccept(AcceptEvent);
}

bool FListener::Run(const FInternetAddr& Addr)
{
	Socket = FSocketSubsystem::CreateSocket();
	if (!Socket)
	{
		return false;
	}

	if (GCompletionPort.Enqueue(this) == false)
	{
		return false;
	}

	if (Socket->SetReuseAddr() == false)
	{
		return false;
	}

	if (Socket->SetLinger(false, 0) == false)
	{
		return false;
	}

	if (Socket->Bind(Addr) == false)
	{
		return false;
	}

	if (Socket->Listen() == false)
	{
		return false;
	}

	const int32 NumAccepts = 1;
	for (int32 i = 0; i < NumAccepts; ++i)
	{
		FOverlapped_Accept* Event = New<FOverlapped_Accept>();
		AcceptEvents.emplace_back(Event);
		RegisterAccept(Event);
	}

	return true;
}

void FListener::Stop()
{
	Socket->Close();
}

void FListener::RegisterAccept(FOverlapped_Accept* Event)
{
	FSession* Session = New<FSession>();

	Event->Init();
	Event->SetSession(Session);

	DWORD BytesReceived = 0;

	bool Result = FSocketSubsystem::Accept(
		Socket->GetNativeSocket(),
		Session->GetSocket()->GetNativeSocket(),
		/*first block of data sent on a new connection*/Session->RecvBuf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&BytesReceived,
		static_cast<LPOVERLAPPED>(Event)
	);

	if (Result == false)
	{
		int32 Error = ::WSAGetLastError();
		if (Error != WSA_IO_PENDING)
		{
			RegisterAccept(Event);
		}
	}
}

void FListener::ProcessAccept(FOverlapped_Accept* Event)
{
	FSession* Session = Event->GetSession();
	if (false == Session->GetSocket()->SetUpdateAcceptSocket(Socket))
	{
		RegisterAccept(Event);
		return;
	}

	SOCKADDR_IN Addr;
	int32 AddrLen = sizeof(Addr);
	if (SOCKET_ERROR == ::getpeername(
		Session->GetSocket()->GetNativeSocket(),
		reinterpret_cast<SOCKADDR*>(&Addr),
		&AddrLen))
	{
		RegisterAccept(Event);
		return;
	}

	Session->SetIpAddress(Addr);
	cout << "Client Connected!" << endl;
	// TODO

	RegisterAccept(Event);
}
