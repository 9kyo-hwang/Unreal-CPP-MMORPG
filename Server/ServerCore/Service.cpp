#include "pch.h"
#include "Service.h"

#include "Listener.h"
#include "Session.h"

FService::FService(EServiceType InType, FInternetAddr InAddr, shared_ptr<FSocketEventQueue> InEventQueue,
                   FSessionFactory InSessionFactory, int32 InNumMaxSessions)
	: Type(InType)
	, Addr(InAddr)
	, EventQueue(InEventQueue)
	, NumSessions(0)
	, NumMaxSessions(InNumMaxSessions)
	, SessionFactory(InSessionFactory)
{
}

FService::~FService()
{
}

shared_ptr<FSession> FService::CreateSession()
{
	shared_ptr<FSession> NewSession = SessionFactory();
	NewSession->SetService(AsShared());
	if (EventQueue->Enqueue(NewSession) == false)
	{
		return nullptr;
	}

	return NewSession;
}

void FService::AddSession(shared_ptr<FSession> InSession)
{
	WRITE_LOCK;
	++NumSessions;
	Sessions.emplace(InSession);
}

void FService::RemoveSession(shared_ptr<FSession> InSession)
{
	WRITE_LOCK;
	check(Sessions.erase(InSession) != 0);
	--NumSessions;
}

void FService::Broadcast(shared_ptr<FSendBuffer> SendBuffer)
{
	WRITE_LOCK;
	for (const auto& Session : Sessions)
	{
		Session->Send(SendBuffer);
	}
}

FClientService::FClientService(FInternetAddr TargetAddr, shared_ptr<FSocketEventQueue> InEventQueue, FSessionFactory InSessionFactory, int32 InNumMaxSessions)
	: Super(EServiceType::Client, TargetAddr, InEventQueue, InSessionFactory, InNumMaxSessions)
{
	
}

FClientService::~FClientService()
{
}

bool FClientService::Run()
{
	if (!CanRun())
	{
		return false;
	}

	const int32 NumSessions = GetNumMaxSessions();
	for (int32 i = 0; i < NumSessions; ++i)
	{
		shared_ptr<FSession> NewSession = CreateSession();
		if (NewSession->Connect() == false)
		{
			return false;
		}
	}

	return true;
}

void FClientService::Stop()
{
	
}

FServerService::FServerService(FInternetAddr InAddr, shared_ptr<FSocketEventQueue> InEventQueue, FSessionFactory InSessionFactory, int32 InNumMaxSessions)
	: Super(EServiceType::Server, InAddr, InEventQueue, InSessionFactory, InNumMaxSessions)
{

}

FServerService::~FServerService()
{

}

bool FServerService::Run()
{
	// TODO

	if (CanRun() == false)
	{
		return false;
	}

	Listener = MakeShared<FListener>();
	if (Listener == nullptr)
	{
		return false;
	}

	// 현재 FInternetAddr을 받고 있는데, ServerService로 변경할 예정
	if (false == Listener->Run(SharedThis(this)))
	{
		return false;
	}

	return true;
}

void FServerService::Stop()
{
	// TODO
}
