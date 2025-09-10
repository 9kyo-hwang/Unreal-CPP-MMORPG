#include "pch.h"
#include "TaskQueue.h"
#include "TaskDispatcher.h"

/*--------------
	FTaskQueue
---------------*/

void FTaskQueue::AddTask(FTaskRef InTask, bool bPushOnly)
{
	const int32 PrevCount = NumTask.fetch_add(1);
	Tasks.Push(InTask); // WRITE_LOCK

	// 첫번째 Job을 넣은 쓰레드가 실행까지 담당
	if (PrevCount == 0)
	{
		// 이미 실행중인 TaskQueue가 없으면 실행
		if (LCurrentTaskQueue == nullptr && bPushOnly == false)
		{
			Execute();
		}
		else
		{
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			GTaskQueueDispatcher->Push(AsShared());
		}
	}
}

// 1) 일감이 너~무 몰리면?
void FTaskQueue::Execute()
{
	LCurrentTaskQueue = this;

	while (true)
	{
		TArray<FTaskRef> TasksToExecute;
		Tasks.PopAll(TasksToExecute);

		const int32 NumCurrentTask = static_cast<int32>(TasksToExecute.size());
		for (int32 i = 0; i < NumCurrentTask; i++)
			TasksToExecute[i]->Execute();

		// 남은 일감이 0개라면 종료
		if (NumTask.fetch_sub(NumCurrentTask) == NumCurrentTask)
		{
			LCurrentTaskQueue = nullptr;
			return;
		}

		const uint64 Tick = ::GetTickCount64();
		if (Tick >= LEndTickCount)
		{
			LCurrentTaskQueue = nullptr;
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			GTaskQueueDispatcher->Push(AsShared());
			break;
		}			
	}
}
