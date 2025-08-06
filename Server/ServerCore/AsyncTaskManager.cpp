#include "pch.h"
#include "AsyncTaskManager.h"

#include "GlobalAsyncTaskManager.h"

void FAsyncTaskQueue::Add(shared_ptr<FTask>&& Task)
{
	const int32 Prev = NumTasks.fetch_add(1);	// 반드시 카운트 증가가 선행
	Tasks.Enqueue(Task);	// WRITE_LOCK

	if (Prev == 0)	// 최초로 Task를 추가한 상태 -> 내가 실행까지 담당
	{
		if (LAsyncTaskQueue == nullptr)
		{
			Launch();
		}
		else
		{
			GAsyncTaskQueueManager->AddQueue(AsShared());
		}
	}
}

void FAsyncTaskQueue::Launch()
{
	LAsyncTaskQueue = this;
	while (true)
	{
		TArray<shared_ptr<FTask>> TaskList;
		Tasks.Dequeue(TaskList);
		const int32 NumQueuedTask = TaskList.size();

		for (const auto& Task : TaskList)
		{
			Task->Launch();
		}

		// 줄이기 전 개수가 정확히 큐에 들어있던 일감 개수와 동일했다면
		if (NumQueuedTask == NumTasks.fetch_sub(NumQueuedTask))	// 반드시 카운트 감소가 후행
		{
			LAsyncTaskQueue = nullptr;
			break;
		}

		const uint64 Tick = ::GetTickCount64();
		if (Tick >= LEndTick)
		{
			LAsyncTaskQueue = nullptr;
			GAsyncTaskQueueManager->AddQueue(AsShared());
			break;
		}
	}
}
