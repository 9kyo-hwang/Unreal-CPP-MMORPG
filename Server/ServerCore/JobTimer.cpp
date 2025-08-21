#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

/*--------------
	FJobTimer
---------------*/

void FJobTimer::Reserve(uint64 InRate, weak_ptr<FJobQueue> InOwner, FJobRef InJob)
{
	const uint64 ExecuteTick = ::GetTickCount64() + InRate;
	FJobData* Data = new FJobData(InOwner, InJob);

	FScopeLock ScopeLock(CriticalSection);

	Items.push(FTimerItem{ ExecuteTick, Data });
}

void FJobTimer::Distribute(uint64 InTick)
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
		if (FJobQueueRef Owner = Item.Data->Owner.lock())
		{
			Owner->Push(Item.Data->Job);
		}

		delete Item.Data;		
	}

	// 끝났으면 풀어준다
	bIsDistributing.store(false);
}

void FJobTimer::Clear()
{
	FScopeLock ScopeLock(CriticalSection);

	while (Items.empty() == false)
	{
		const FTimerItem& Item = Items.top();
		delete Item.Data;
		Items.pop();
	}
}
