#include "pch.h"
#include <thread>
#include "CoreGlobal.h"
#include "ThreadManager.h"

void ThreadMain()
{
	while (true)
	{
		cout << "[" << LThreadID << "] Thread" << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->AddThread(ThreadMain);
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
