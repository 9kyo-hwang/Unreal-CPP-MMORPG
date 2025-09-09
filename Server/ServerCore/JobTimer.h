#pragma once

struct FJobData
{
	FJobData(TWeakPtr<FJobQueue> InOwner, FJobRef InJob)
		: Owner(InOwner)
		, Job(InJob)
	{

	}

	TWeakPtr<FJobQueue> Owner;
	FJobRef Job;
};

struct FTimerItem
{
	bool operator<(const FTimerItem& Other) const
	{
		return ExecuteTick > Other.ExecuteTick;
	}

	uint64 ExecuteTick = 0;
	FJobData* Data = nullptr;
};

/*--------------
	FJobTimer
---------------*/

class FJobTimer
{
public:
	void Reserve(uint64 InRate, TWeakPtr<FJobQueue> InOwner, FJobRef InJob);
	void Distribute(uint64 InTick);
	void Clear();

private:
	FCriticalSection CriticalSection;
	priority_queue<FTimerItem> Items;
	TAtomic<bool> bIsDistributing{false};
};

