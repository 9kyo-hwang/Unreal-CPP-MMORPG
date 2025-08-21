#pragma once

/*----------------
	ISocketIOEventable
-----------------*/

class ISocketIOEventable : public TSharedFromThis<ISocketIOEventable>
{
public:
	virtual HANDLE GetHandle() = 0;
	virtual void Dispatch(class FSocketIOEvent* iocpEvent, int32 numOfBytes = 0) = 0;
};

/*--------------
	FSocketIOEventQueue
---------------*/

class FSocketIOEventQueue
{
public:
	FSocketIOEventQueue();
	~FSocketIOEventQueue();

	HANDLE		GetHandle() { return Handle; }

	bool		Register(ISocketIOEventableRef iocpObject);
	bool		Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE		Handle;
};