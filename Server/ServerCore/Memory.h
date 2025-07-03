#pragma once

class FMalloc
{
public:
	static void* Malloc(size_t Count);
	static void Free(void* Original);
};

template<typename T, typename... Args>
T* NewObject(Args&&... MallocArgs)
{
	T* Mem = static_cast<T*>(FMalloc::Malloc(sizeof(T)));

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
	FMalloc::Free(Object);
}