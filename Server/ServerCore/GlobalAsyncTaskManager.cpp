#include "pch.h"
#include "GlobalAsyncTaskManager.h"

FAsyncTaskQueueManager::FAsyncTaskQueueManager()
{
}

FAsyncTaskQueueManager::~FAsyncTaskQueueManager()
{
}

void FAsyncTaskQueueManager::AddQueue(shared_ptr<FAsyncTaskQueue> InQueue)
{
	TaskQueues.Enqueue(InQueue);
}

shared_ptr<FAsyncTaskQueue> FAsyncTaskQueueManager::RemoveQueue()
{
	return TaskQueues.Dequeue();
}
