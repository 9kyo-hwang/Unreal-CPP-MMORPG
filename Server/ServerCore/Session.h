#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"
#include "Socket.h"

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
	void Send(FSendBufferRef InSendBuffer);
	bool Connect();
	void Disconnect(const WCHAR* Msg);

	TSharedPtr<FService> GetService() const { return Service.lock(); }
	void SetService(TSharedPtr<FService> InService) { Service = InService; }

public:
	void				SetNetAddress(FNetAddress InAddr) { NetAddr = InAddr; }
	FNetAddress			GetAddress() const { return NetAddr; }
	SOCKET				GetSocket() const { return Socket.GetSocket(); }
	bool				IsConnected() { return bIsConnected; }
	FSessionRef			GetSessionRef() { return SharedThis<FSession>(this); }

	bool SetUpdateAcceptSocket(SOCKET ListenSocket)
	{
		return Socket.SetUpdateAcceptSocket(ListenSocket);
	}

private:
	HANDLE GetHandle() override;
	void Dispatch(FSocketIOEvent* InEvent, int32 NumOfBytes = 0) override;

private:
	bool RegisterConnect();
	bool RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();
		 
	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 BytesRecvd);
	void ProcessSend(int32 BytesSent);
		 
	void HandleError(int32 ErrorCode);

protected:
	virtual void OnConnected() { }
	virtual int32 OnRecv(BYTE* InBuffer, int32 InLength) { return InLength; }
	virtual void OnSend(int32 InLength) { }
	virtual void OnDisconnected() { }

private:
	TWeakPtr<FService>	Service;
	FSocket				Socket;
	FNetAddress			NetAddr{};
	TAtomic<bool>		bIsConnected{false};

private:
	FCriticalSection CriticalSection;
	FReceiveBuffer RecvBuffer;
	TQueue<FSendBufferRef> SendQueue;
	TAtomic<bool> bIsSendRegistered{false};

private:
	FConnectEvent		ConnectEvent;
	FDisconnectEvent	DisconnectEvent;
	FRecvEvent			RecvEvent;
	FSendEvent			SendEvent;
};

/*-----------------
	FPacketSession
------------------*/

struct PacketHeader
{
	uint16 Size;
	uint16 Id;
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