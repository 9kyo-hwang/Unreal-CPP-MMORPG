#pragma once

#include "Allocator.h"

template<typename T, typename... Args>
T* NewObject(Args&&... MallocArgs)
{
	T* Mem = static_cast<T*>(FMallocStomp::Malloc(sizeof(T)));

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
	FMallocStomp::Free(Object);
}