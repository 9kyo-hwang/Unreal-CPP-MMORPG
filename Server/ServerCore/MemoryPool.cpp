#include "pch.h"
#include "MemoryPool.h"

FMemoryPool::FMemoryPool(int32 InSize)
	: Size(InSize)
{
	::InitializeSListHead(&ListHead);
}

FMemoryPool::~FMemoryPool()
{
	while (FAllocationData* AllocDataPtr = static_cast<FAllocationData*>(::InterlockedPopEntrySList(&ListHead)))
	{
		AlignedFree(AllocDataPtr);
	}
}

void FMemoryPool::Push(FAllocationData* InAllocDataPtr)
{
	// Pool에 반납
	InAllocDataPtr->Size = 0;
	::InterlockedPushEntrySList(&ListHead, InAllocDataPtr);	// ListEntry를 상속받아서 캐스팅 불필요
	Num.fetch_sub(1);
}

FAllocationData* FMemoryPool::Pop()
{
	// Pool에서 꺼내오기

	FAllocationData* AllocDataPtr = static_cast<FAllocationData*>(::InterlockedPopEntrySList(&ListHead));

	if (!AllocDataPtr)
	{
		// TEMP
		AllocDataPtr = static_cast<FAllocationData*>(AlignedMalloc(Size, Alignment));
	}
	else
	{
		check(AllocDataPtr->Size == 0);
	}

	Num.fetch_add(1);
	return AllocDataPtr;
}
