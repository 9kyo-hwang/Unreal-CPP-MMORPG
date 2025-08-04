#pragma once

class FTask;
class FTaskQueue
{
public:
	void Enqueue(shared_ptr<FTask> Task)
	{
		WRITE_LOCK;
		Tasks.emplace(Task);
	}

	shared_ptr<FTask> Dequeue()
	{
		WRITE_LOCK;
		if (Tasks.empty())
		{
			return nullptr;
		}

		shared_ptr<FTask> Task = Tasks.front();
		Tasks.pop();
		return Task;
	}

private:
	USE_LOCK;
	TQueue<shared_ptr<FTask>> Tasks;
};