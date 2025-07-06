#pragma once

#include "Allocator.h"

class FMemoryPool;

class FMemory
{
	// ~1024까지는 32 단위, ~2048까지는 128 단위, ~4096까지는 256 단위
	static constexpr uint8 PoolCount = (1024 / 32) + (1024 / 128) + (2048 / 256);
	static constexpr uint32 MaxAllocSize = 4096;

public:
	FMemory();
	~FMemory();

	void* Malloc(int32 Size);
	void Free(void* InPtr);

private:
	vector<FMemoryPool*> Pools;
	FMemoryPool* PoolTable[MaxAllocSize + 1];
};


template<typename T, typename... Args>
T* NewObject(Args&&... MallocArgs)
{
	T* Mem = static_cast<T*>(FMallocStomp::Malloc(sizeof(T)));

	/**
	 *	malloc을 이용한 메모리 할당은 클래스 생성자를 자동으로 호출하지 않음
	 *	따라서 명시적으로 생성자 호출을 수행해줘야 함
	 *	Placement New
	 */

	new(Mem)T(forward<Args>(MallocArgs)...);

	return Mem;
}

template<typename T>
void DeleteObject(T* Object)
{
	Object->~T();
	FMallocStomp::Free(Object);
}