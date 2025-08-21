#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

/*-------------
	FService
--------------*/

FService::FService(EServiceType InType, NetAddress InAddr, FSocketIOEventQueueRef InEventQueue, FSessionFactory InFactory, int32 InMaxSessionCount)
	: Type(InType), Addr(InAddr), EventQueue(InEventQueue), MaxSessionCount(InMaxSessionCount), Factory(InFactory)
{

}

FService::~FService()
{
}

void FService::CloseService()
{
	// TODO
}

void FService::Broadcast(FSendBufferRef InSendBuffer)
{
	FScopeLock ScopeLock(CriticalSection);
	for (const auto& Session : Sessions)
	{
		Session->Send(InSendBuffer);
	}
}

FSessionRef FService::CreateSession()
{
	FSessionRef Session = Factory();
	Session->SetService(AsShared());

	if (EventQueue->Register(Session) == false)
	{
		return nullptr;
	}

	return Session;
}

void FService::AddSession(FSessionRef NewSession)
{
	FScopeLock ScopeLock(CriticalSection);
	SessionCount++;
	Sessions.insert(NewSession);
}

void FService::ReleaseSession(FSessionRef TargetSession)
{
	FScopeLock ScopeLock(CriticalSection);
	check(Sessions.erase(TargetSession) != 0);
	SessionCount--;
}

/*-----------------
	FClientService
------------------*/

FClientService::FClientService(NetAddress InTargetAddr, FSocketIOEventQueueRef InEventQueue, FSessionFactory InFactory, int32 InMaxSessionCount)
	: FService(EServiceType::Client, InTargetAddr, InEventQueue, InFactory, InMaxSessionCount)
{
}

bool FClientService::Start()
{
	if (CanStart() == false)
	{
		return false;
	}

	for (int32 i = 0; i < GetMaxSessionCount(); i++)
	{
		FSessionRef Session = CreateSession();
		if (Session->Connect() == false)
		{
			return false;
		}
	}

	return true;
}

FServerService::FServerService(NetAddress address, FSocketIOEventQueueRef InEventQueue, FSessionFactory InFactory, int32 InMaxSessionCount)
	: FService(EServiceType::Server, address, InEventQueue, InFactory, InMaxSessionCount)
{
}

bool FServerService::Start()
{
	if (CanStart() == false)
	{
		return false;
	}

	Listener = make_shared<FListener>();
	if (Listener == nullptr)
	{
		return false;
	}

	auto Service = SharedThis<FServerService>(this);
	if (Listener->StartAccept(Service) == false)
	{
		return false;
	}

	return true;
}

void FServerService::CloseService()
{
	// TODO
	FService::CloseService();
}
