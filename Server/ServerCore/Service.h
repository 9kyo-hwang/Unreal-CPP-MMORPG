#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"

enum class EServiceType : uint8
{
	Server,
	Client
};

/*-------------
	FService
--------------*/

using FSessionFactory = TFunction<FSessionRef(void)>;

class FService : public TSharedFromThis<FService>
{
public:
	FService(EServiceType InType, NetAddress InAddr, FSocketIOEventQueueRef InEventQueue, FSessionFactory InFactory, int32 InMaxSessionCount = 1);
	virtual ~FService();

	virtual bool		Start() = 0;
	bool				CanStart() const { return Factory != nullptr; }

	virtual void		CloseService();
	void				SetSessionFactory(FSessionFactory InFactory) { Factory = InFactory; }

	void				Broadcast(FSendBufferRef InSendBuffer);
	FSessionRef			CreateSession();
	void				AddSession(FSessionRef NewSession);
	void				ReleaseSession(FSessionRef TargetSession);
	int32				GetCurrentSessionCount() const { return SessionCount; }
	int32				GetMaxSessionCount() const { return MaxSessionCount; }

public:
	EServiceType GetServiceType() const { return Type; }
	NetAddress GetNetAddress() const { return Addr; }
	FSocketIOEventQueueRef&	GetEventQueue() { return EventQueue; }

protected:
	FCriticalSection CriticalSection;
	EServiceType		Type;
	NetAddress			Addr{};
	FSocketIOEventQueueRef	EventQueue;

	TSet<FSessionRef>	Sessions;
	int32				SessionCount = 0;
	int32				MaxSessionCount = 0;
	FSessionFactory		Factory;
};

/*-----------------
	FClientService
------------------*/

class FClientService : public FService
{
public:
	FClientService(NetAddress InTargetAddr, FSocketIOEventQueueRef InEventQueue, FSessionFactory InFactory, int32 InMaxSessionCount = 1);
	~FClientService() override {}

	bool	Start() override;
};


/*-----------------
	FServerService
------------------*/

class FServerService : public FService
{
public:
	FServerService(NetAddress InTargetAddr, FSocketIOEventQueueRef InEventQueue, FSessionFactory InFactory, int32 InMaxSessionCount = 1);
	~FServerService() override {}

	bool	Start() override;
	void	CloseService() override;

private:
	FListenerRef Listener = nullptr;
};