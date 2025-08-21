#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class FService;

/*--------------
	FSession
---------------*/

class FSession : public ISocketIOEventable
{
	friend class FListener;
	friend class FSocketIOEventQueue;
	friend class FService;

	enum
	{
		BUFFER_SIZE = 0x10000, // 64KB
	};

public:
	FSession();
	virtual ~FSession();

public:
						/* 외부에서 사용 */
	void				Send(FSendBufferRef InSendBuffer);
	bool				Connect();
	void				Disconnect(const WCHAR* Msg);

	shared_ptr<FService> GetService() const { return Service.lock(); }
	void SetService(shared_ptr<FService> InService) { Service = InService; }

public:
						/* 정보 관련 */
	void				SetNetAddress(NetAddress InAddr) { NetAddr = InAddr; }
	NetAddress			GetAddress() const { return NetAddr; }
	SOCKET				GetSocket() const { return Socket; }
	bool				IsConnected() { return bIsConnected; }
	FSessionRef			GetSessionRef() { return SharedThis<FSession>(this); }

private:
						/* 인터페이스 구현 */
	HANDLE		GetHandle() override;
	void		Dispatch(FSocketIOEvent* InEvent, int32 NumOfBytes = 0) override;

private:
						/* 전송 관련 */
	bool				RegisterConnect();
	bool				RegisterDisconnect();
	void				RegisterRecv();
	void				RegisterSend();

	void				ProcessConnect();
	void				ProcessDisconnect();
	void				ProcessRecv(int32 BytesRecvd);
	void				ProcessSend(int32 BytesSent);

	void				HandleError(int32 ErrorCode);

protected:
						/* 컨텐츠 코드에서 재정의 */
	virtual void		OnConnected() { }
	virtual int32		OnRecv(BYTE* InBuffer, int32 InLength) { return InLength; }
	virtual void		OnSend(int32 InLength) { }
	virtual void		OnDisconnected() { }

private:
	weak_ptr<FService>	Service;
	SOCKET				Socket = INVALID_SOCKET;
	NetAddress			NetAddr = {};
	atomic<bool>		bIsConnected{false};

private:
	FCriticalSection CriticalSection;
	/* 수신 관련 */
	FReceiveBuffer RecvBuffer;
	/* 송신 관련 */
	TQueue<FSendBufferRef> SendQueue;
	atomic<bool> bIsSendRegistered{false};

private:
						/* FSocketIOEvent 재사용 */
	FConnectEvent		ConnectEvent;
	FDisconnectEvent	DisconnectEvent;
	FRecvEvent			RecvEvent;
	FSendEvent			SendEvent;
};

/*-----------------
	FPacketSession
------------------*/

struct FPacketHeader
{
	uint16 Size;
	uint16 Id; // 프로토콜ID (ex. 1=로그인, 2=이동요청)
};

class FPacketSession : public FSession
{
public:
	FPacketSession();
	~FPacketSession() override;

	FPacketSessionRef GetPacketSessionRef() { return SharedThis<FPacketSession>(this); }

protected:
	int32 OnRecv(BYTE* InBuffer, int32 InLength) sealed;
	virtual void OnReceive(BYTE* InBuffer, int32 InLength) = 0;
};