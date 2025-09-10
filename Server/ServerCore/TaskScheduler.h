#pragma once

struct FTaskData
{
	FTaskData(TWeakPtr<FTaskQueue> InOwner, FTaskRef InTask)
		: Owner(InOwner)
		, Task(InTask)
	{

	}

	TWeakPtr<FTaskQueue> Owner;
	FTaskRef Task;
};

struct FTimerItem
{
	bool operator<(const FTimerItem& Other) const
	{
		return ExecuteTick > Other.ExecuteTick;
	}

	uint64 ExecuteTick = 0;
	FTaskData* Data = nullptr;
};

/*--------------
	FTaskScheduler
---------------*/

class FTaskScheduler
{
public:
	void Register(uint64 InRate, TWeakPtr<FTaskQueue> InOwner, FTaskRef InTask);
	void DispatchReadyTasks(uint64 InTick);
	void Clear();

private:
	FCriticalSection CriticalSection;
	priority_queue<FTimerItem> Items;
	TAtomic<bool> bIsDistributing{false};
};

