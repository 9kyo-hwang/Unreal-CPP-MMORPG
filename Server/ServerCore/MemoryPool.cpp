#include "pch.h"
#include "MemoryPool.h"

FMemoryPool::FMemoryPool(int32 InSize)
	: MallocSize(InSize)
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

void FMemoryPool::Release(FAllocationData* InAllocDataPtr)
{
	// Pool에 반납
	InAllocDataPtr->Size = 0;
	::InterlockedPushEntrySList(&ListHead, InAllocDataPtr);	// ListEntry를 상속받아서 캐스팅 불필요
	NumUses.fetch_sub(1);
	NumReserved.fetch_add(1);
}

FAllocationData* FMemoryPool::Get()
{
	// Pool에서 꺼내오기
	FAllocationData* AllocDataPtr = static_cast<FAllocationData*>(::InterlockedPopEntrySList(&ListHead));

	if (!AllocDataPtr)
	{
		// TEMP
		AllocDataPtr = static_cast<FAllocationData*>(AlignedMalloc(MallocSize, Alignment));
	}
	else
	{
		check(AllocDataPtr->Size == 0);
		NumReserved.fetch_sub(1);
	}

	NumUses.fetch_add(1);
	return AllocDataPtr;
}