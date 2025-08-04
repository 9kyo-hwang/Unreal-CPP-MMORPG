#pragma once
#include "Task.h"
#include "TasksSystem.h"

class FTaskArchive : public TSharedFromThis<FTaskArchive>
{
public:
	virtual ~FTaskArchive() = default;

	void Serialize(CallbackType&& Callback)
	{
		auto Task = TObjectPool<FTask>::MakeShared(move(Callback));
		Queue.Enqueue(Task);
	}

	template<typename ClassType, typename ReturnType, typename... MethodArgs>
	void Serialize(ReturnType(ClassType::*Method)(MethodArgs...), MethodArgs... Args)
	{
		auto Owner = StaticCastSharedPtr<ClassType>(AsShared());
		auto Task = TObjectPool<FTask>::MakeShared(Owner, Method, forward<MethodArgs>(Args)...);
		Queue.Enqueue(Task);
	}

	virtual void Flush() = 0;

protected:
	FTaskQueue Queue;
};

