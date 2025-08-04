#pragma once
#include <functional>

using CallbackType = function<void()>;

class FTask
{
public:
	FTask(CallbackType&& InCallback)
		: Callback(move(InCallback))
	{}

	template<typename ClassType, typename ReturnType, typename... CallbackArgs>
	FTask(shared_ptr<ClassType> Owner, ReturnType(ClassType::*Method)(CallbackArgs...), CallbackArgs&&... Args)
	{
		Callback = [Owner, Method, Args...]()
			{
				(Owner.get()->*Method)(Args...);
			};
	}

	void Launch() const
	{
		Callback();
	}

private:
	CallbackType Callback;
};

