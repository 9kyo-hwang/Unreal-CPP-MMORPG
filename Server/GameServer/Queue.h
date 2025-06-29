#pragma once

#include <mutex>

template<typename T>
class TQueue
{
public:
	TQueue() { }
	TQueue(const TQueue&) = delete;
	TQueue& operator=(const TQueue&) = delete;

	void Emplace(T InValue)
	{
		lock_guard<mutex> Lock(Mutex);
		Data.emplace(move(InValue));
		ConditionVariable.notify_one();
	}

	bool TryGet(T& OutValue)
	{
		lock_guard<mutex> Lock(Mutex);
		if (Data.empty())
		{
			return false;
		}

		OutValue = move(Data.front());
		Data.pop();
		return true;
	}

	T TryGet()
	{
		unique_lock<mutex> Lock(Mutex);
		ConditionVariable.wait(Lock, [this]() { return !Data.empty(); });	// Queue에 데이터가 있을 때까지 대기

		T Value = move(Data.front());
		Data.pop();
		return Value;
	}

private:
	queue<T> Data;
	mutex Mutex;
	condition_variable ConditionVariable;
};

