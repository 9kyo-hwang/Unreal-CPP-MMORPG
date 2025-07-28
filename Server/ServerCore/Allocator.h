#pragma once

#include "pch.h"

class FMallocStomp
{
    constexpr static size_t PageSize = 0x1000;	// 페이지 크기(4byte)의 배수

public:
    static void* Malloc(size_t Size);
    static void Free(void* InPtr);
};

class FMallocPool
{
public:
    static void* Malloc(size_t Size);
    static void Free(void* InPtr);
};

template <typename T>
class TAllocator
{
public:
    using value_type = T;

    TAllocator() noexcept {}
    template <typename U>
    TAllocator(const TAllocator<U>&) noexcept {}

    T* allocate(size_t NumBytes)
	{
        int32 Size = static_cast<int32>(NumBytes * sizeof(T));
        return static_cast<T*>(FMallocPool::Malloc(Size));
    }

    void deallocate(T* Memory, size_t) noexcept
	{
        FMallocPool::Free(Memory);
    }
};
