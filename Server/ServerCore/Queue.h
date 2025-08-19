#pragma once

template<typename T>
class TQueue
{
public:
	TQueue() = default;
	~TQueue() = default;

	void Enqueue(T Item)
	{
		WRITE_LOCK;
		Data.push(Item);
	}

	T Dequeue()
	{
		WRITE_LOCK;
		if (Data.empty())
		{
			return T();
		}

		T Item = Data.front();
		Data.pop();
		return Item;
	}

	void Dequeue(vector<T>& OutItems)
	{
		WRITE_LOCK;
		while (T Item = Dequeue())
		{
			OutItems.push_back(Item);
		}
	}

	void Empty()
	{
		WRITE_LOCK;
		Data = queue<T>();
	}

private:
	USE_LOCK;
	queue<T> Data;
};