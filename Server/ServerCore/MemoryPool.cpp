#include "pch.h"
#include "MemoryPool.h"

FMemoryPool::FMemoryPool(int32 InSize)
	: Size(InSize)
{
}

FMemoryPool::~FMemoryPool()
{
	while (!Queue.empty())
	{
		FAllocationData* AllocDataPtr = Queue.front();
		Queue.pop();

		::free(AllocDataPtr);
	}
}

void FMemoryPool::Push(FAllocationData* InAllocDataPtr)
{
	// Pool¿¡ ¹Ý³³

	WRITE_LOCK;

	InAllocDataPtr->Size = 0;
	Queue.push(InAllocDataPtr);
	Num.fetch_sub(1);
}

FAllocationData* FMemoryPool::Pop()
{
	// Pool¿¡¼­ ²¨³»¿À±â

	FAllocationData* AllocDataPtr = nullptr;
	{
		WRITE_LOCK;
		if (!Queue.empty())
		{
			AllocDataPtr = Queue.front();
			Queue.pop();
		}
	}

	if (!AllocDataPtr)
	{
		// TEMP
		AllocDataPtr = static_cast<FAllocationData*>(::malloc(Size));
	}
	else
	{
		check(AllocDataPtr->Size == 0);
	}

	Num.fetch_add(1);
	return AllocDataPtr;
}
