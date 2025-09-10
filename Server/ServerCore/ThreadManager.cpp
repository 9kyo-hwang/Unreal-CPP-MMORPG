#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "TaskDispatcher.h"

/*------------------
	FThreadManager
-------------------*/

FThreadManager::FThreadManager()
{
	// Main Thread
	InitTLS();
}

FThreadManager::~FThreadManager()
{
	Join();
}

void FThreadManager::Launch(function<void(void)> Callable)
{
	FScopeLock ScopeLock(CriticalSection);

	Threads.push_back(thread([=]()
		{
			InitTLS();
			Callable();
			DestroyTLS();
		}));
}

void FThreadManager::Join()
{
	for (thread& Thread : Threads)
	{
		if (Thread.joinable())
			Thread.join();
	}
	Threads.clear();
}

void FThreadManager::InitTLS()
{
	static TAtomic<uint32> SThreadId{1};
	LThreadId = SThreadId.fetch_add(1);
}

void FThreadManager::DestroyTLS()
{

}

void FThreadManager::ProcessTaskQueues()
{
	while (true)
	{
		uint64 Tick = ::GetTickCount64();
		if (Tick > LEndTickCount)
		{
			break;
		}

		FTaskQueueRef TaskQueue = GTaskQueueDispatcher->Pop();
		if (TaskQueue == nullptr)
		{
			break;
		}

		TaskQueue->Execute();
	}
}

void FThreadManager::FlushTaskScheduler()
{
	const uint64 Tick = ::GetTickCount64();

	GTaskScheduler->DispatchReadyTasks(Tick);
}
