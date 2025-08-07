#pragma once

class FTask;

struct FTaskData
{
	FTaskData(weak_ptr<FAsyncTaskQueue> InOwner, shared_ptr<FTask> InTask)
		: Owner(InOwner)
		, Task(InTask)
	{}

	weak_ptr<FAsyncTaskQueue> Owner;	// no shared_ptr: Timer 대기 시간동안 소멸되지 못하는 단점을 회피
	shared_ptr<FTask> Task;
};

struct FTimerHandle
{
	FTimerHandle(uint64 InExecuteTick = 0, FTaskData* InData = nullptr)
		: ExecuteTick(InExecuteTick)
		, Data(InData)
	{}

	friend bool operator<(const FTimerHandle& Lhs, const FTimerHandle& Rhs)
	{
		// PQ는 거꾸로
		return Lhs.ExecuteTick > Rhs.ExecuteTick;
	}

	uint64 ExecuteTick;	// 실행될 시점
	FTaskData* Data;	// PQ 내부적으로 위치가 바뀔 때마다 shared_ptr의 참조 횟수가 바뀌는 것을 막기 위해
};

class FTaskTimerManager
{
public:
	void SetTimer(uint64 InRate, weak_ptr<FAsyncTaskQueue> InOwner, shared_ptr<FTask> InTask);
	void Distribute(uint64 Now);	// ExecuteTick이 된 Item들을 OwnerQueue에다가 Task를 넣어주는 역할
	void Clear();

private:
	USE_LOCK;
	TPriorityQueue<FTimerHandle> Handles;
	TAtomic<bool> bDistributing;	// 현재 PQ Item을 재배치하고 있는지
};
