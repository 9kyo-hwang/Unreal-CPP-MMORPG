#pragma once
#include "Task.h"
#include "TaskTimer.h"

class FAsyncTaskQueue : public TSharedFromThis<FAsyncTaskQueue>
{
public:
	virtual ~FAsyncTaskQueue() = default;

	void Add(CallableType&& InCallable)
	{
		Add(TObjectPool<FTask>::MakeShared(move(InCallable)));
	}

	template<typename ClassType, typename ReturnType, typename... MethodArgs>
	void Add(ReturnType(ClassType::*Method)(MethodArgs...), MethodArgs... Args)
	{
		auto Owner = StaticCastSharedPtr<ClassType>(AsShared());
		Add(TObjectPool<FTask>::MakeShared(Owner, Method, forward<MethodArgs>(Args)...));
	}

	void AddTimer(uint64 InRate, CallableType&& Callable)
	{
		auto Task = TObjectPool<FTask>::MakeShared(move(Callable));
		GTaskTimerManager->SetTimer(InRate, AsShared(), Task);
	}

	template<typename ClassType, typename ReturnType, typename... MethodArgs>
	void AddTimer(uint64 InRate, ReturnType(ClassType::* Method)(MethodArgs...), MethodArgs... Args)
	{
		auto Owner = StaticCastSharedPtr<ClassType>(AsShared());
		auto Task = TObjectPool<FTask>::MakeShared(Owner, Method, forward<MethodArgs>(Args)...);
		GTaskTimerManager->SetTimer(InRate, Owner, Task);
	}

	void Empty() { Tasks.Empty(); }
	void Add(shared_ptr<FTask> Task, bool bDoLaunch = true);
	void Launch();

protected:
	TQueue<shared_ptr<FTask>> Tasks;
	TAtomic<int32> NumTasks;
};

