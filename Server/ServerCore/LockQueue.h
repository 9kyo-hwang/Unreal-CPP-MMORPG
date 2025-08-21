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
		if (Items.empty())
			return T();

		T ret = Items.front();
		Items.pop();
		return ret;
	}

	void PopAll(vector<T>& OutItems)
	{
		FScopeLock ScopeLock(CriticalSection);
		while (T Item = Pop())
			OutItems.push_back(Item);
	}

	void Clear()
	{
		FScopeLock ScopeLock(CriticalSection);
		Items = TQueue<T>();
	}

private:
	FCriticalSection CriticalSection;
	TQueue<T> Items;
};