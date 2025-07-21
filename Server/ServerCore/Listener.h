#pragma once
#include "IOCPCore.h"

class FInternetAddr;
class FSocket;
class FSocketAccept;
class FServerService;

class FListener : public ISocketEventable
{
public:
	FListener();
	~FListener();

	HANDLE GetHandle() override;
	void Dispatch(FSocketEvent* Event, int32 NumBytes = 0) override;

	bool Run(shared_ptr<FServerService> InServerService);
	void Stop();

private:
	void RegisterAccept(FSocketAccept* Event);
	void ProcessAccept(FSocketAccept* Event);

protected:
	unique_ptr<FSocket> Socket;
	TArray<FSocketAccept*> AcceptEvents;
	weak_ptr<FServerService> ServerService;	// Listener가 속한 서비스
};

