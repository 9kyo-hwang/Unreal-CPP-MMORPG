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
	 *	malloc�� �̿��� �޸� �Ҵ��� Ŭ���� �����ڸ� �ڵ����� ȣ������ ����
	 *	���� ��������� ������ ȣ���� ��������� ��
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