#pragma once
#include <functional>

using CallableType = function<void()>;

class FTask
{
public:
	FTask(CallableType&& InCallable)
		: Callable(move(InCallable))
	{}

	template<typename ClassType, typename ReturnType, typename... CallbackArgs>
	FTask(shared_ptr<ClassType> Owner, ReturnType(ClassType::*Method)(CallbackArgs...), CallbackArgs&&... Args)
	{
		Callable = [Owner, Method, Args...]()
			{
				(Owner.get()->*Method)(Args...);
			};
	}

	void Launch() const
	{
		Callable();
	}

private:
	CallableType Callable;
};

