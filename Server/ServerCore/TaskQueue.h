#pragma once
#include "Task.h"
#include "LockQueue.h"
#include "TaskScheduler.h"

class FTaskQueue : public TSharedFromThis<FTaskQueue>
{
public:
	void Post(CallableType&& InCallable)
	{
		AddTask(MakeShared<FTask>(std::move(InCallable)));
	}

	//template<typename ClassType, typename ReturnType, typename... Args>
	//void Post(ReturnType(ClassType::*Method)(Args...), Args... InArgs)
	//{
	//	TSharedPtr<ClassType> Owner = StaticCastSharedPtr<ClassType>(AsShared());
	//	AddTask(MakeShared<FTask>(Owner, Method, std::forward<Args>(InArgs)...));
	//}

	template<typename ClassType, typename ReturnType, typename... Args>
	void Post(TSharedPtr<ClassType> InOwner, ReturnType(ClassType::* Method)(Args...), Args... InArgs)
	{
		AddTask(MakeShared<FTask>(InOwner, Method, std::forward<Args>(InArgs)...));
	}

	void PostAfter(uint64 InRate, CallableType&& InCallable)
	{
		FTaskRef Task = MakeShared<FTask>(std::move(InCallable));
		GTaskScheduler->Register(InRate, AsShared(), Task);
	}

	//template<typename ClassType, typename ReturnType, typename... Args>
	//void PostAfter(uint64 InRate, ReturnType(ClassType::* Method)(Args...), Args... InArgs)
	//{
	//	TSharedPtr<ClassType> Owner = StaticCastSharedPtr<ClassType>(AsShared());
	//	FTaskRef Task = MakeShared<FTask>(Owner, Method, std::forward<Args>(InArgs)...);
	//	GTaskScheduler->Register(InRate, AsShared(), Task);
	//}

	template<typename ClassType, typename ReturnType, typename... Args>
	void PostAfter(uint64 InRate, TSharedPtr<ClassType> InOwner, ReturnType(ClassType::* Method)(Args...), Args... InArgs)
	{
		FTaskRef Task = MakeShared<FTask>(InOwner, Method, std::forward<Args>(InArgs)...);
		GTaskScheduler->Register(InRate, AsShared(), Task);
	}

	void Clear() { Tasks.Clear(); }

public:
	void AddTask(FTaskRef InTask, bool bPushOnly = false);
	void Execute();

protected:
	TLockQueue<FTaskRef> Tasks;
	TAtomic<int32> NumTask{0};
};

