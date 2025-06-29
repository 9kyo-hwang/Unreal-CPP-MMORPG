#include "pch.h"
#include "ThreadManager.h"

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
