#pragma once
#include "Task.h"

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

	void Empty() { Tasks.Empty(); }

private:
	void Add(shared_ptr<FTask>&& Task);

public:
	void Launch();

protected:
	TQueue<shared_ptr<FTask>> Tasks;
	TAtomic<int32> NumTasks;
};

