#pragma once

/*---------
	FJob
----------*/

using CallableType = TFunction<void()>;

class FJob
{
public:
	FJob(CallableType&& InCallable) : Callable(std::move(InCallable))
	{
	}

	template<typename T, typename Ret, typename... Args>
	FJob(TSharedPtr<T> Owner, Ret(T::* Method)(Args...), Args&&... InArgs)
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

