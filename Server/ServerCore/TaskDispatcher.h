#pragma once

class FTaskQueueDispatcher
{
public:
	FTaskQueueDispatcher();
	~FTaskQueueDispatcher();

	void Push(FTaskQueueRef InQueue);
	FTaskQueueRef Pop();

private:
	TLockQueue<FTaskQueueRef> TaskQueues;
};