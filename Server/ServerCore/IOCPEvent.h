#pragma once

enum class EIoEvent : uint8
{
	Connect,
	Accept,
	// PreRecv,
	Recv,
	Send
};

class FOverlapped : public OVERLAPPED
{
	using Super = OVERLAPPED;

public:
	FOverlapped(EIoEvent InEventType);

	void Init();
	EIoEvent GetEventType() const
	{
		return EventType;
	}

private:
	EIoEvent EventType;
};

// Warning: OVERLAPPED 구조체가 메모리 최상단에 위치하도록 virtual 선언을 해서는 안됨!
class FOverlapped_Connect : public FOverlapped
{
	using Super = FOverlapped;

public:
	FOverlapped_Connect();
};

class FSession;
class FOverlapped_Accept : public FOverlapped
{
	using Super = FOverlapped;

public:
	FOverlapped_Accept();
	FSession* GetSession();
	void SetSession(FSession* InSession);

private:
	// TODO: AcceptEx가 필요로 하는 추가 정보(Session)
	FSession* Session;
};

class FOverlapped_Recv : public FOverlapped
{
	using Super = FOverlapped;

public:
	FOverlapped_Recv();
};

class FOverlapped_Send : public FOverlapped
{
	using Super = FOverlapped;

public:
	FOverlapped_Send();
};
