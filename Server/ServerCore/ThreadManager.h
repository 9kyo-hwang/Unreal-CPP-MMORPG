#pragma once

#include <thread>
#include <functional>

class FThreadManager
{
public:
	FThreadManager();
	~FThreadManager();

	void AddThread(function<void(void)> Func);
	void WaitForCompletion();

	static void SetTls();
	static void FreeTls();
	static void QueueAsyncTask();
	static void DistributeReservedTasks();

private:
	FCriticalSection Mutex;
	vector<thread> Threads;
};
