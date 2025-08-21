#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class FAcceptEvent;
class FServerService;

/*--------------
	FListener
---------------*/

class FListener : public ISocketIOEventable
{
public:
	FListener() = default;
	~FListener();

public:
	/* 외부에서 사용 */
	bool StartAccept(FServerServiceRef InService);
	void CloseSocket();

public:
	/* 인터페이스 구현 */
	HANDLE GetHandle() override;
	void Dispatch(FSocketIOEvent* InEvent, int32 NumOfBytes = 0) override;

private:
	/* 수신 관련 */
	void RegisterAccept(FAcceptEvent* InEvent);
	void ProcessAccept(FAcceptEvent* InEvent);

protected:
	SOCKET Socket = INVALID_SOCKET;
	vector<FAcceptEvent*> AcceptEvents;
	FServerServiceRef Service;
};

