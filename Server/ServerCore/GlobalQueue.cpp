#include "pch.h"
#include "GlobalQueue.h"

/*----------------
	FGlobalJobQueue
-----------------*/

FGlobalJobQueue::FGlobalJobQueue()
{

}

FGlobalJobQueue::~FGlobalJobQueue()
{

}

void FGlobalJobQueue::Push(FJobQueueRef InQueue)
{
	JobQueues.Push(InQueue);
}

FJobQueueRef FGlobalJobQueue::Pop()
{
	return JobQueues.Pop();
}
