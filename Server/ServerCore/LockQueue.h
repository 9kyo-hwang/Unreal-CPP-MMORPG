#pragma once

template<typename T>
class TLockQueue
{
public:
	void Push(T Item)
	{
		FScopeLock ScopeLock(CriticalSection);
		Items.push(Item);
	}

	T Pop()
	{
		FScopeLock ScopeLock(CriticalSection);
		return PopInternal();
	}

	void PopAll(TArray<T>& OutItems)
	{
		FScopeLock ScopeLock(CriticalSection);
		while (T Item = PopInternal())
		{
			OutItems.push_back(Item);
		}
	}

	void Clear()
	{
		FScopeLock ScopeLock(CriticalSection);
		Items = TQueue<T>();
	}

private:
	T PopInternal()
	{
		if (Items.empty())
		{
			return T();
		}

		T RetVal = Items.front();
		Items.pop();
		return RetVal;
	}

private:
	FCriticalSection CriticalSection;
	TQueue<T> Items;
};