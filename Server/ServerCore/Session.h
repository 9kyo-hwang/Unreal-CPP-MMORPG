#pragma once
#include "IOCPCore.h"
#include "IPAddress.h"

class FSocket;
class FInternetAddr;

// 클라이언트의 모든 정보를 들고 있는 클래스(CP에 등록될 객체)
class FSession : public ICompletion
{
public:
	FSession();
	virtual ~FSession();

	HANDLE GetHandle() override;
	void Dispatch(FOverlapped* Event, int32 NumBytes = 0) override;

	// Session 정보 관련
	void SetIpAddress(FInternetAddr InAddr) { Addr = InAddr; }
	FInternetAddr GetIpAddress() const { return Addr; }
	FSocket* GetSocket() const { return Socket.get(); }

public:
	// TEMP: 임시로 Send/Receive 버퍼 정의
	char RecvBuf[1000];

private:
	unique_ptr<FSocket> Socket;
	FInternetAddr Addr;
	TAtomic<bool> bIsConnected;
};

