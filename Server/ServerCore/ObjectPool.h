#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename T>
class TObjectPool
{
public:
	template<typename... PoolArgs>
	static T* Get(PoolArgs&&... Args)
	{
#ifdef _STOMP
		FAllocationData* AllocDataPtr = static_cast<FAllocationData*>(FMallocStomp::Malloc(AllocSize));
		T* Mem = static_cast<T*>(FAllocationData::Attach(AllocDataPtr, AllocSize));
#else
		T* Mem = static_cast<T*>(FAllocationData::Attach(Pool.Get(), AllocSize));
#endif
		
		new(Mem)T(forward<PoolArgs>(Args)...);
		return Mem;
	}

	static void Release(T* Object)
	{
		Object->~T();

#ifdef _STOMP
		FMallocStomp::Free(FAllocationData::Detach(Object));
#else
		Pool.Release(FAllocationData::Detach(Object));
#endif
	}

	static shared_ptr<T> MakeShared()
	{
		return shared_ptr<T>{ Get(), Release };
	}

private:
	static int32 AllocSize;
	static FMemoryPool Pool;
};

template<typename T>
int32 TObjectPool<T>::AllocSize = sizeof(T) + sizeof(FMemoryPool);
template<typename T>
FMemoryPool TObjectPool<T>::Pool = { AllocSize };