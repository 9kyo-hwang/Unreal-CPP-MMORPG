#pragma once

class FSession;

enum class EEventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv,
	Recv,
	Send
};

/*--------------
	FSocketIOEvent
---------------*/

class FSocketIOEvent : public OVERLAPPED
{
public:
	FSocketIOEvent(EEventType type);

	void			Init();

public:
	EEventType		EventType;
	ISocketIOEventableRef	Owner;
};

/*----------------
	FConnectEvent
-----------------*/

class FConnectEvent : public FSocketIOEvent
{
public:
	FConnectEvent() : FSocketIOEvent(EEventType::Connect) { }
};

/*--------------------
	FDisconnectEvent
----------------------*/

class FDisconnectEvent : public FSocketIOEvent
{
public:
	FDisconnectEvent() : FSocketIOEvent(EEventType::Disconnect) { }
};

/*----------------
	FAcceptEvent
-----------------*/

class FAcceptEvent : public FSocketIOEvent
{
public:
	FAcceptEvent() : FSocketIOEvent(EEventType::Accept) { }

public:
	FSessionRef	Session = nullptr;
};

/*----------------
	FRecvEvent
-----------------*/

class FRecvEvent : public FSocketIOEvent
{
public:
	FRecvEvent() : FSocketIOEvent(EEventType::Recv) { }
};

/*----------------
	FSendEvent
-----------------*/

class FSendEvent : public FSocketIOEvent
{
public:
	FSendEvent() : FSocketIOEvent(EEventType::Send) { }
	 
	TArray<FSendBufferRef> SendBuffers;
};