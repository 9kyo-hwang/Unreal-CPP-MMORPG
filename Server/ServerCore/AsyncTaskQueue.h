#pragma once
#include "Task.h"
#include "TaskTimer.h"
#include "Queue.h"

class FAsyncTaskQueue : public enable_shared_from_this<FAsyncTaskQueue>
{
public:
	virtual ~FAsyncTaskQueue() = default;

	void Add(CallableType&& Callable)
	{
		Add(make_shared<FTask>(move(Callable)));
	}

	template<typename ClassType, typename ReturnType, typename... MethodArgs>
	void Add(ReturnType(ClassType::*Method)(MethodArgs...), MethodArgs... Args)
	{
		shared_ptr<ClassType> Owner = static_pointer_cast<ClassType>(shared_from_this());
		Add(make_shared<FTask>(Owner, Method, forward<MethodArgs>(Args)...));
	}

	template<typename ClassType, typename ReturnType, typename... MethodArgs>
	void AddTimer(uint64 InRate, ReturnType(ClassType::* Method)(MethodArgs...), MethodArgs... Args)
	{
		shared_ptr<ClassType> Owner = static_pointer_cast<ClassType>(shared_from_this());
		auto Task = make_shared<FTask>(Owner, Method, std::forward<MethodArgs>(Args)...);
		GTaskTimerManager->SetTimer(InRate, shared_from_this(), Task);
	}

	void Empty() { Tasks.Empty(); }
	void Launch();
	void Add(shared_ptr<FTask> Task, bool bDoLaunch = true);

protected:
	TQueue<shared_ptr<FTask>> Tasks;
	TAtomic<int32> NumTasks;
};

