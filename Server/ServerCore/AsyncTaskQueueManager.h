#pragma once
#include "Queue.h"

class FAsyncTaskQueue;

class FAsyncTaskQueueManager
{
public:
	FAsyncTaskQueueManager();
	~FAsyncTaskQueueManager();

	void AddQueue(shared_ptr<FAsyncTaskQueue> InQueue);
	shared_ptr<FAsyncTaskQueue> RemoveQueue();

private:
	TQueue<shared_ptr<FAsyncTaskQueue>> TaskQueues;
};

