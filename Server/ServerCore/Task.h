#pragma once
#include <functional>

using CallableType = std::function<void()>;

class FTask
{
public:
	FTask(CallableType&& InCallable)
		: Callable(move(InCallable))
	{}

	template<typename ClassType, typename ReturnType, typename... MethodArgs>
	FTask(shared_ptr<ClassType> Owner, ReturnType(ClassType::* Method)(MethodArgs...), MethodArgs&&... Args)
	{
		Callable = [Owner, Method, Args...]()
			{
				(Owner.get()->*Method)(Args...);
			};
	}

	void Execute()
	{
		Callable();
	}

private:
	CallableType Callable;
};