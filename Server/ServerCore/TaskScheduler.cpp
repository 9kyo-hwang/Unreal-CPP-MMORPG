#include "pch.h"
#include "TaskScheduler.h"
#include "TaskQueue.h"

/*--------------
	FTaskScheduler
---------------*/

void FTaskScheduler::Register(uint64 InRate, TWeakPtr<FTaskQueue> InOwner, FTaskRef InTask)
{
	const uint64 ExecuteTick = ::GetTickCount64() + InRate;
	FTaskData* Data = new FTaskData(InOwner, InTask);

	FScopeLock ScopeLock(CriticalSection);

	Items.push(FTimerItem{ ExecuteTick, Data });
}

void FTaskScheduler::DispatchReadyTasks(uint64 InTick)
{
	// 한 번에 1 쓰레드만 통과
	if (bIsDistributing.exchange(true) == true)
	{
		return;
	}

	vector<FTimerItem> CandidateItems;
	{
		FScopeLock ScopeLock(CriticalSection);

		while (Items.empty() == false)
		{
			const FTimerItem& Item = Items.top();
			if (InTick < Item.ExecuteTick)
				break;

			CandidateItems.push_back(Item);
			Items.pop();
		}
	}

	for (FTimerItem& Item : CandidateItems)
	{
		if (FTaskQueueRef Owner = Item.Data->Owner.lock())
		{
			Owner->AddTask(Item.Data->Task);
		}

		delete Item.Data;		
	}

	// 끝났으면 풀어준다
	bIsDistributing.store(false);
}

void FTaskScheduler::Clear()
{
	FScopeLock ScopeLock(CriticalSection);

	while (Items.empty() == false)
	{
		const FTimerItem& Item = Items.top();
		delete Item.Data;
		Items.pop();
	}
}
