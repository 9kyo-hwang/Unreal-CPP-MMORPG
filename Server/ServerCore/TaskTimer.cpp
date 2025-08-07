#include "pch.h"
#include "TaskTimer.h"

#include "AsyncTaskQueue.h"

void FTaskTimerManager::SetTimer(uint64 InRate, weak_ptr<FAsyncTaskQueue> InOwner, shared_ptr<ITask> InTask)
{
	const uint64 ExecuteTick = ::GetTickCount64() + InRate;	// 경합 X
	FTaskData* TaskData = TObjectPool<FTaskData>::Get(InOwner, InTask);	// 이미 Lock

	WRITE_LOCK;
	Handles.emplace(ExecuteTick, TaskData);
}

void FTaskTimerManager::Distribute(uint64 Now)
{
	// 한 번에 한 쓰레드만
	if (bDistributing.exchange(true) == true)
	{
		return;
	}

	TArray<FTimerHandle> HandlesToBeExecuted;
	{
		WRITE_LOCK;

		while (!Handles.empty())
		{
			const FTimerHandle& Handle = Handles.top();
			if (Now < Handle.ExecuteTick)
			{
				break;
			}

			HandlesToBeExecuted.push_back(Handle);
			Handles.pop();
		}
	}

	// Lag 등의 이유로, 낮은 확률로 예약한 순서와 맞지 않게 Task를 처리할 수 있음 -> TAtomic을 사용하는 이유
	for (FTimerHandle& Handle : HandlesToBeExecuted)
	{
		if (auto Owner = Handle.Data->Owner.lock())
		{
			Owner->Add(Handle.Data->Task);
		}

		TObjectPool<FTaskData>::Release(Handle.Data);
	}

	bDistributing.store(false);
}

// 전역으로 존재할 예정이라 Clear를 호출할 일은 없음
void FTaskTimerManager::Clear()
{
	WRITE_LOCK;

	while (!Handles.empty())
	{
		const FTimerHandle& Handle = Handles.top();
		TObjectPool<FTaskData>::Release(Handle.Data);
		Handles.pop();
	}
}
