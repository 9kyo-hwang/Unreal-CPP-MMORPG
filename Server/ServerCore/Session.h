#pragma once
#include "IOCPCore.h"
#include "IOCPEvent.h"
#include "IPAddress.h"
#include "RecvBuffer.h"

class FSocket;
class FInternetAddr;

// 클라이언트의 모든 정보를 들고 있는 클래스(CP에 등록될 객체)
class FSession : public ISocketEventable
{
	friend class FListener;
	friend class FSocketEventQueue;
	friend class FService;

	static constexpr int32 BufferSize = 0x10000; // 64KB

public:
	FSession();
	virtual ~FSession();

	// Session 정보 관련
	void SetIpAddress(FInternetAddr InAddr) { Addr = InAddr; }

	FInternetAddr GetIpAddress() const { return Addr; }
	FSocket* GetSocket() const { return Socket.get(); }
	shared_ptr<FSession> GetSession() { return static_pointer_cast<FSession>(shared_from_this()); }
	shared_ptr<FService> GetService() const { return Service.lock(); }

	bool IsConnected() const { return bIsConnected; }
	void SetService(shared_ptr<FService> InService) { Service = InService; }

	void Send(shared_ptr<FSendBuffer> SendBuffer);
	bool Connect();
	void Disconnect(const TCHAR* Msg);

private:
	HANDLE GetHandle() override;
	void Dispatch(FSocketEvent* Event, int32 NumOfBytes = 0) override;

	// 전송 관련 메서드
	bool RegisterConnect();	// Client Server 단에서 Connect를 등록할 수 있음
	bool RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 BytesToRecv);
	void ProcessSend(int32 BytesToSend);

	void HandleError(int32 Error);

protected:
	// 컨텐츠단에서 재정의해서 사용
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* Buffer, int32 Length) { return Length; }
	virtual void OnSend(int32 BytesSent) {}
	virtual void OnDisconnected() {}

private:
	weak_ptr<FService> Service;	// Session이 속한 서비스
	unique_ptr<FSocket> Socket;
	FInternetAddr Addr;
	TAtomic<bool> bIsConnected;

private:
	USE_LOCK;

	// Recv
	FRecvBuffer RecvBuffer;
	queue<shared_ptr<FSendBuffer>> SendQueue;	// Send 이벤트가 여러 개 등록될 수 있으므로 Queue로 관리
	TAtomic<bool> bIsSending;	// 현재 Send 이벤트가 진행 중인지 여부

	// Send

private:  // Event Reuse
	FSocketConnect ConnectEvent;
	FSocketDisconnect DisconnectEvent;
	FSocketRecv RecvEvent;
	FSocketSend SendEvent;
};

struct FPacketHeader
{
	// uint32로 4byte x 2 해도 가능
	uint16 Id;		// Protocol Id(1=Login, 2=Move, ...)
	uint16 Size;	// Total Packet Size
};

// 컨텐츠 단에서는 반드시 이 패킷 세션을 상속받아 사용해야 함
class FPacketSession : public FSession
{
	using Super = FSession;

public:
	FPacketSession();
	~FPacketSession() override;

	shared_ptr<FPacketSession> SharedThisSession() { return static_pointer_cast<FPacketSession>(shared_from_this()); }

protected:
	int32 OnRecv(BYTE* Buffer, int32 Length) sealed;	// 하위 클래스에서 사용하지 못하도록
	virtual void OnReceive(BYTE* Buffer, int32 Length) = 0;
};