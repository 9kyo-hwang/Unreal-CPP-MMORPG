#pragma once

enum class ESocketEventTypes : uint8
{
	Connect,
	Accept,
	// PreRecv,
	Recv,
	Send
};

class FSocketEvent : public OVERLAPPED
{
	using Super = OVERLAPPED;

public:
	FSocketEvent(ESocketEventTypes InType);

	void Init();

	ESocketEventTypes Type;
	shared_ptr<class ISocketEventable> Owner;
};

// Warning: OVERLAPPED 구조체가 메모리 최상단에 위치하도록 virtual 선언을 해서는 안됨!
class FSocketConnect : public FSocketEvent
{
	using Super = FSocketEvent;

public:
	FSocketConnect();
};

class FSession;
class FSocketAccept : public FSocketEvent
{
	using Super = FSocketEvent;

public:
	FSocketAccept();

	// TODO: AcceptEx가 필요로 하는 추가 정보(Session)
	shared_ptr<FSession> Session;
};

class FSocketRecv : public FSocketEvent
{
	using Super = FSocketEvent;

public:
	FSocketRecv();
};

class FSocketSend : public FSocketEvent
{
	using Super = FSocketEvent;

public:
	FSocketSend();

	// TEMP
	vector<BYTE> Buffer;
};
