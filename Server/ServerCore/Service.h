#pragma once
#include <functional>

#include "IPAddress.h"
#include "IOCPCore.h"

enum class EServiceType
{
	Server,
	Client,
};

using FSessionFactory = function<shared_ptr<class FSession>(void)>;

// Client가 될 수도, Server가 될 수도, 다양한 정책을 가질 수 있는 클래스
class FService : public TSharedFromThis<FService>
{
public:
	FService(
		EServiceType InType,
		FInternetAddr InAddr,
		shared_ptr<FSocketEventQueue> InEventQueue,
		FSessionFactory InSessionFactory,
		int32 InNumMaxSessions = 1
	);

	virtual ~FService();

	virtual bool Run() = 0;
	bool CanRun() const { return SessionFactory != nullptr; }

	virtual void Stop() = 0;
	void SetSessionFactory(FSessionFactory InSessionFactory) { SessionFactory = InSessionFactory; }

	shared_ptr<FSession> CreateSession();
	void AddSession(shared_ptr<FSession> InSession);
	void RemoveSession(shared_ptr<FSession> InSession);

	EServiceType GetType() const { return Type; }
	FInternetAddr GetAddr() const { return Addr; }
	shared_ptr<FSocketEventQueue>& GetEventQueue() { return EventQueue; }

	// LOCK이 걸렸기 때문에 해당 메서드로 얻는 수치는 정확한 수치가 아닐 수 있음
	int32 GetNumSessions() const { return NumSessions; }
	int32 GetNumMaxSessions() const { return NumMaxSessions; }

protected:
	USE_LOCK;

	EServiceType Type;
	FInternetAddr Addr;
	shared_ptr<FSocketEventQueue> EventQueue;

	TSet<shared_ptr<FSession>> Sessions;
	int32 NumSessions;
	int32 NumMaxSessions;
	FSessionFactory SessionFactory;
};

class FClientService : public FService
{
	using Super = FService;

public:
	FClientService(
		FInternetAddr TargetAddr,
		shared_ptr<FSocketEventQueue> InEventQueue,
		FSessionFactory InSessionFactory,
		int32 InNumMaxSessions = 1
	);
	~FClientService() override;

	bool Run() override;
	void Stop() override;
};

class FServerService : public FService
{
	using Super = FService;

public:
	FServerService(
		FInternetAddr InAddr,
		shared_ptr<FSocketEventQueue> InEventQueue,
		FSessionFactory InSessionFactory,
		int32 InNumMaxSessions = 1
	);

	~FServerService() override;

	bool Run() override;
	void Stop() override;

private:
	shared_ptr<class FListener> Listener;
};