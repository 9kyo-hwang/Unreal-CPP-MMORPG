#pragma once

/*----------------
	FGlobalJobQueue
-----------------*/

class FGlobalJobQueue
{
public:
	FGlobalJobQueue();
	~FGlobalJobQueue();

	void Push(FJobQueueRef InQueue);
	FJobQueueRef Pop();

private:
	TLockQueue<FJobQueueRef> JobQueues;
};

