#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

/*--------------
	FJobQueue
---------------*/

class FJobQueue : public TSharedFromThis<FJobQueue>
{
public:
	void DoAsync(CallableType&& InCallable)
	{
		Push(make_shared<FJob>(std::move(InCallable)));
	}

	template<typename ClassType, typename ReturnType, typename... Args>
	void DoAsync(ReturnType(ClassType::*Method)(Args...), Args... InArgs)
	{
		shared_ptr<ClassType> Owner = SharedThis<ClassType>(this);
		Push(make_shared<FJob>(Owner, Method, std::forward<Args>(InArgs)...));
	}

	void DoTimer(uint64 InRate, CallableType&& InCallable)
	{
		FJobRef Job = make_shared<FJob>(std::move(InCallable));
		GJobTimer->Reserve(InRate, AsShared(), Job);
	}

	template<typename ClassType, typename ReturnType, typename... Args>
	void DoTimer(uint64 InRate, ReturnType(ClassType::* Method)(Args...), Args... InArgs)
	{
		shared_ptr<ClassType> Owner = SharedThis<ClassType>(this);
		FJobRef Job = make_shared<FJob>(Owner, Method, std::forward<Args>(InArgs)...);
		GJobTimer->Reserve(InRate, AsShared(), Job);
	}

	void					Clear() { Jobs.Clear(); }

public:
	void					Push(FJobRef InJob, bool bPushOnly = false);
	void					Execute();

protected:
	TLockQueue<FJobRef>		Jobs;
	atomic<int32>			JobCount{0};
};

