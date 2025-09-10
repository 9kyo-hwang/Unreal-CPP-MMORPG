#include "pch.h"
#include "TaskDispatcher.h"

/*----------------
	FTaskQueueDispatcher
-----------------*/

FTaskQueueDispatcher::FTaskQueueDispatcher()
{

}

FTaskQueueDispatcher::~FTaskQueueDispatcher()
{

}

void FTaskQueueDispatcher::Push(FTaskQueueRef InQueue)
{
	TaskQueues.Push(InQueue);
}

FTaskQueueRef FTaskQueueDispatcher::Pop()
{
	return TaskQueues.Pop();
}
