#pragma once
#include "IOCPCore.h"
#include "IOCPEvent.h"
#include "IPAddress.h"

class FSocket;
class FInternetAddr;

// 클라이언트의 모든 정보를 들고 있는 클래스(CP에 등록될 객체)
class FSession : public ISocketEventable
{
	friend class FListener;
	friend class FSocketEventQueue;
	friend class FService;

public:
	FSession();
	virtual ~FSession();

	// Session 정보 관련
	void SetIpAddress(FInternetAddr InAddr) { Addr = InAddr; }

	FInternetAddr GetIpAddress() const { return Addr; }
	FSocket* GetSocket() const { return Socket.get(); }
	shared_ptr<FSession> GetSession() { return SharedThis(this); }
	shared_ptr<FService> GetService() const { return Service.lock(); }

	bool IsConnected() const { return bIsConnected; }
	void SetService(shared_ptr<FService> InService) { Service = InService; }

	void Disconnect(const TCHAR* Msg);

private:
	HANDLE GetHandle() override;
	void Dispatch(FSocketEvent* Event, int32 NumOfBytes = 0) override;

	// 전송 관련 메서드
	void RegisterConnect();	// Client Server 단에서 Connect를 등록할 수 있음
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessRecv(int32 BytesRecvd);
	void ProcessSend(int32 BytesSent);

	void HandleError(int32 Error);

protected:
	// 컨텐츠 단에서 오버로딩해서 사용할 것
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* Buffer, int32 BufferLength) { return BufferLength; }
	virtual void OnSend(int32 BytesSent) {}
	virtual void OnDisconnected() {}

public:
	// TEMP: 임시로 Send/Receive 버퍼 정의
	char RecvBuf[1000];

private:
	weak_ptr<FService> Service;	// Session이 속한 서비스
	unique_ptr<FSocket> Socket;
	FInternetAddr Addr;
	TAtomic<bool> bIsConnected;

private:
	USE_LOCK;

	// Recv

	// Send

private:  // Event Reuse
	FSocketRecv RecvEvent;
};

