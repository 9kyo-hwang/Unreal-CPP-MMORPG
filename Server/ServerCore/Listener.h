#pragma once
#include "IocpCore.h"
#include "NetAddress.h"
#include "FSocket.h"

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
	/* �ܺο��� ��� */
	bool StartAccept(FServerServiceRef InService);
	void CloseSocket();

public:
	/* �������̽� ���� */
	HANDLE GetHandle() override;
	void Dispatch(FSocketIOEvent* InEvent, int32 NumOfBytes = 0) override;

private:
	/* ���� ���� */
	void RegisterAccept(FAcceptEvent* InEvent);
	void ProcessAccept(FAcceptEvent* InEvent);

protected:
	FSocket Socket;
	TArray<FAcceptEvent*> AcceptEvents;
	FServerServiceRef Service;
};

