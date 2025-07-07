#include "pch.h"
#include "MemoryBase.h"
#include "MemoryPool.h"

FMemory::FMemory()
	: PoolTable{}
{
	uint32 PoolIndex = 0;
	for (uint32 Size = 32; Size <= 1024; Size += 32)
	{
		FMemoryPool* Pool = new FMemoryPool(Size);
		Pools.push_back(Pool);

		for (; PoolIndex <= Size; ++PoolIndex)
		{
			PoolTable[PoolIndex] = Pool;
		}
	}

	for (uint32 Size = 1024; Size <= 2048; Size += 128)
	{
		FMemoryPool* Pool = new FMemoryPool(Size);
		Pools.push_back(Pool);

		for (; PoolIndex <= Size; ++PoolIndex)
		{
			PoolTable[PoolIndex] = Pool;
		}
	}

	for (uint32 Size = 2048; Size <= 4096; Size += 256)
	{
		FMemoryPool* Pool = new FMemoryPool(Size);
		Pools.push_back(Pool);

		for (; PoolIndex <= Size; ++PoolIndex)
		{
			PoolTable[PoolIndex] = Pool;
		}
	}
}

FMemory::~FMemory()
{
	for (FMemoryPool* Pool : Pools)
	{
		delete Pool;
	}

	Pools.clear();
}

void* FMemory::Malloc(int32 Size)
{
	// 각자의 메모리 영역만 건드리고 있기 때문에, 별도로 LOCK을 걸지 않음
	FAllocationData* Data = nullptr;
	const int32 AllocSize = Size + sizeof(FAllocationData);

	if (AllocSize > MaxAllocSize)
	{
		// 메모리 풀링 최대 크기보다 크면 그냥 할당
		Data = static_cast<FAllocationData*>(AlignedMalloc(AllocSize, Alignment));
	}
	else
	{
		// 해당 사이즈의 풀에서 Pop
		Data = PoolTable[AllocSize]->Pop();
	}

	// 내부적으로 AllocSize를 헤더 형식으로 기입해준 뒤, 실제 데이터 시작 주소를 반환해줌
	return FAllocationData::Attach(Data, AllocSize);
}

void FMemory::Free(void* InPtr)
{
	// 그리고 Pool에 Push/Pop할 때 내부적으로 LOCK을 걸어주고 있음
	if (InPtr == nullptr)
	{
		return; 
	}

	FAllocationData* AllocDataPtr = static_cast<FAllocationData*>(InPtr);
	AllocDataPtr--;

	const int32 AllocSize = AllocDataPtr->Size;
	check(AllocSize > 0);

	if (AllocSize > MaxAllocSize)
	{
		// 메모리 풀링 최대 크기를 벗어남 -> free 호출
		AlignedFree(AllocDataPtr);
	}
	else
	{
		// 메모리 풀에 반납
		PoolTable[AllocSize]->Push(AllocDataPtr);
	}
}
