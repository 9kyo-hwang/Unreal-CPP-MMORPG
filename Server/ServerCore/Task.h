#pragma once

/*---------
	FTask
----------*/

using CallableType = TFunction<void()>;

class FTask
{
public:
	FTask(CallableType&& InCallable) : Callable(std::move(InCallable))
	{
	}

	template<typename T, typename Ret, typename... Args>
	FTask(TSharedPtr<T> Owner, Ret(T::* Method)(Args...), Args&&... InArgs)
	{
		Callable = [Owner, Method, InArgs...]()
		{
			(Owner.get()->*Method)(InArgs...);
		};
	}

	void Execute() const
	{
		Callable();
	}

private:
	CallableType Callable;
};

