#pragma once
#include "MemoryBase.h"

// GetQueuedCompletionStatus()에서 추가 정보를 받기 위해 Key와 Overlapped 구조체를 상속/합성해서 사용
class FSocketEvent;				// Overlapped 구조체를 상속받은, IO 이벤트에 대응하는 클래스
class ISocketEventable : public TSharedFromThis<ISocketEventable>	// (Session의 베이스)
{
public:
	virtual HANDLE GetHandle() = 0;
	virtual void Dispatch(FSocketEvent* Event, int32 NumBytes = 0) = 0;
};

class FSocketEventQueue
{
public:
	FSocketEventQueue();
	~FSocketEventQueue();

	HANDLE GetData() const
	{
		return Data;
	}

	bool Enqueue(shared_ptr<ISocketEventable> Socket);
	bool Dequeue(uint32 TimeoutMilliseconds = INFINITE);

private:
	HANDLE Data;
};