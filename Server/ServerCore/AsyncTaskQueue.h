#pragma once
#include "Task.h"
#include "TaskTimer.h"

class FAsyncTaskQueue : public TSharedFromThis<FAsyncTaskQueue>
{
	friend class FTaskTimerManager;

public:
	virtual ~FAsyncTaskQueue() = default;

	template<typename InCallableType>
	void Add(InCallableType&& Callable)
	{
		auto Task = TObjectPool<TTask<InCallableType>>::MakeShared(std::forward<InCallableType>(Callable));
		Add(StaticCastSharedPtr<ITask>(Task));
	}

	template<typename InCallableType>
	void AddTimer(uint64 InRate, InCallableType&& Callable)
	{
		auto Task = TObjectPool<TTask<InCallableType>>::MakeShared(std::forward<InCallableType>(Callable));
		GTaskTimerManager->SetTimer(InRate, AsShared(), StaticCastSharedPtr<ITask>(Task));
	}

	void Empty() { Tasks.Empty(); }
	void Launch();

private:
	void Add(shared_ptr<ITask> Task, bool bDoLaunch = true);

protected:
	TQueue<shared_ptr<ITask>> Tasks;
	TAtomic<int32> NumTasks;
};

