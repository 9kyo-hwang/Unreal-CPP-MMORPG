#pragma once

__interface ITask
{
	void Execute();
};

template<typename InCallableType>
class TTask : public ITask
{
public:
	TTask(InCallableType&& InCallable)
		: Callable(move(InCallable))
	{}

	void Execute() override
	{
		Callable();
	}

private:
	InCallableType Callable;
};

using FTask = TTask<void>;