#include "pch.h"
#include "Listener.h"
#include "IOCPEvent.h"
#include "Service.h"
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

	for (FSocketAccept* Event : AcceptEvents)
	{
		// TODO

		Delete(Event);
	}
}

HANDLE FListener::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket->GetNativeSocket());
}

void FListener::Dispatch(FSocketEvent* Event, int32 NumBytes)
{
	check(Event->Type == ESocketEventTypes::Accept);
	FSocketAccept* AcceptEvent = static_cast<FSocketAccept*>(Event);
	ProcessAccept(AcceptEvent);
}

bool FListener::Run(shared_ptr<FServerService> InServerService)
{
	if (!InServerService)
	{
		return false;
	}

	ServerService = InServerService;

	Socket = FSocketSubsystem::CreateSocket();
	if (!Socket)
	{
		return false;
	}

	if (ServerService.expired())
	{
		return false;
	}

	// 전역 Completion Port를 사용하지 않고, Service가 들고 있는 CP에 접근
	if (ServerService.lock()->GetEventQueue()->Enqueue(AsShared()) == false)
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

	if (Socket->Bind(ServerService.lock()->GetAddr()) == false)
	{
		return false;
	}

	if (Socket->Listen() == false)
	{
		return false;
	}

	const int32 NumAccepts = ServerService.lock()->GetNumMaxSessions();
	for (int32 i = 0; i < NumAccepts; ++i)
	{
		FSocketAccept* Event = New<FSocketAccept>();
		Event->Owner = AsShared();
		AcceptEvents.emplace_back(Event);
		RegisterAccept(Event);
	}

	return true;
}

void FListener::Stop()
{
	Socket->Close();
}

void FListener::RegisterAccept(FSocketAccept* Event)
{
	// Session을 Create할 때 서비스의 CP에 자동으로 등록도 수행해줌
	auto Session = ServerService.lock()->CreateSession();

	Event->Init();
	Event->Session = Session;

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

void FListener::ProcessAccept(FSocketAccept* Event)
{
	auto Session = Event->Session;
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

	// TODO: Recv & Send
	Session->ProcessConnect();

	RegisterAccept(Event);
}
