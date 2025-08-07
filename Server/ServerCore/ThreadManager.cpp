#include "pch.h"
#include "ThreadManager.h"

#include "AsyncTaskQueue.h"
#include "AsyncTaskQueueManager.h"

FThreadManager::FThreadManager()
{
	// Main Thread
	SetTls();
}

FThreadManager::~FThreadManager()
{
	WaitForCompletion();
}

void FThreadManager::AddThread(function<void()> Func)
{
	FScopeLock Lock(Mutex);
	Threads.emplace_back(thread([=]()
		{
			SetTls();
			Func();
			FreeTls();
		}));
}

void FThreadManager::WaitForCompletion()
{
	for (thread& Thread : Threads)
	{
		if (Thread.joinable())
		{
			Thread.join();
		}
	}

	Threads.clear();
}

void FThreadManager::SetTls()
{
	static TAtomic<uint32> SThreadID = 1;
	LThreadID = SThreadID.fetch_add(1);
}

void FThreadManager::FreeTls()
{

}

void FThreadManager::QueueAsyncTask()
{
	while (true)
	{
		uint64 Tick = ::GetTickCount64();
		if (Tick > LEndTick)
		{
			break;
		}

		shared_ptr<FAsyncTaskQueue> TaskQueue = GAsyncTaskQueueManager->RemoveQueue();
		if (TaskQueue == nullptr)
		{
			break;
		}

		TaskQueue->Launch();
		LEndTick += Tick;
	}
}

void FThreadManager::DistributeReservedTasks()
{
	const uint64 Tick = ::GetTickCount64();
	GTaskTimerManager->Distribute(Tick);
}
