#pragma once

#include <thread>
#include <functional>

/*------------------
	FThreadManager
-------------------*/

class FThreadManager
{
public:
	FThreadManager();
	~FThreadManager();

	void	Launch(function<void(void)> Callable);
	void	Join();

	static void InitTLS();
	static void DestroyTLS();

	static void DoGlobalQueueWork();
	static void DistributeReservedJobs();

private:
	FCriticalSection CriticalSection;
	vector<thread>	Threads;
};

