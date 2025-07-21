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
T* New(Args&&... MallocArgs)
{
	T* Mem = static_cast<T*>(FMallocPool::Malloc(sizeof(T)));

	/**
	 *	malloc을 이용한 메모리 할당은 클래스 생성자를 자동으로 호출하지 않음
	 *	따라서 명시적으로 생성자 호출을 수행해줘야 함
	 *	Placement New
	 */

	new(Mem)T(forward<Args>(MallocArgs)...);

	return Mem;
}

template<typename T>
void Delete(T* Object)
{
	Object->~T();
	FMallocPool::Free(Object);
}

template<typename T, typename... SharedArgs>
shared_ptr<T> MakeShared(SharedArgs&&... Args)
{
	return shared_ptr<T>{New<T>(forward<SharedArgs>(Args)...), Delete<T>};
}

template<typename T>
class TSharedFromThis : public std::enable_shared_from_this<T>
{
public:
    std::shared_ptr<T> AsShared()
    {
        return this->shared_from_this();
    }
    std::shared_ptr<const T> AsShared() const
    {
        return this->shared_from_this();
    }
    /** :contentReference[oaicite:0]{index=0} */

    template<typename U>
    std::shared_ptr<U> SharedThis(U* /*ThisPtr*/)
    {
        return std::static_pointer_cast<U>(this->shared_from_this());
    }
    template<typename U>
    std::shared_ptr<const U> SharedThis(const U* /*ThisPtr*/) const
    {
        return std::static_pointer_cast<const U>(this->shared_from_this());
    }
    /** :contentReference[oaicite:1]{index=1} */

    std::weak_ptr<T> AsWeak() noexcept
    {
        return this->weak_from_this();
    }
    std::weak_ptr<const T> AsWeak() const noexcept
    {
        return this->weak_from_this();
    }
    /** :contentReference[oaicite:2]{index=2} */

    bool DoesSharedInstanceExist() const noexcept
    {
        return !this->weak_from_this().expired();
    }

protected:
    TSharedFromThis() noexcept = default;
    TSharedFromThis(const TSharedFromThis&) noexcept = default;
    TSharedFromThis& operator=(const TSharedFromThis&) noexcept = default;
    ~TSharedFromThis() = default;
};