#pragma once

// GetQueuedCompletionStatus()에서 추가 정보를 받기 위해 Key와 Overlapped 구조체를 상속/합성해서 사용
class FOverlapped;	// Overlapped 구조체를 상속받아 추가 정보를 들고 있는 역할
__interface ICompletion	// Key 역할(이전의 Session 역할)
{
	virtual HANDLE GetHandle() = 0;
	virtual void Dispatch(FOverlapped* Event, int32 NumBytes = 0) = 0;
};

class FCompletionPort
{
public:
	FCompletionPort();
	~FCompletionPort();

	HANDLE GetData() const
	{
		return Handle;
	}

	bool Enqueue(ICompletion* Completion);
	bool Dequeue(uint32 TimeoutMilliseconds = INFINITE);

private:
	HANDLE Handle;
};

// TEMP: 임시로 전역 CP 생성
extern FCompletionPort GCompletionPort;