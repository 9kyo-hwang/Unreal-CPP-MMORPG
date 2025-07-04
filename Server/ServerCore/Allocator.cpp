#include "pch.h"
#include "Allocator.h"

void* FMallocStomp::Malloc(SIZE_T Size)
{
	//const int64 PageCount = (Size + PageSize - 1) / PageSize;
	//const int64 Offset = PageCount * PageSize - Size;
	//void* Base = ::VirtualAlloc(nullptr, PageCount * PageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//return static_cast<int8*>(Base) + Offset;

	const SIZE_T AlignedSize = Size;
	// == ((Size + PageSize - 1) / PageSize) * PageSize
	const SIZE_T AllocFullPageSize = (AlignedSize + PageSize - 1) & -(SSIZE_T)PageSize;

	void* FullAllocationPointer = ::VirtualAlloc(nullptr, AllocFullPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	void* ReturnedPointer = (uint8*)FullAllocationPointer + AllocFullPageSize - AlignedSize;

	return ReturnedPointer;
}

void FMallocStomp::Free(void* InPtr)
{
	if (InPtr == nullptr)
	{
		return;
	}

	const int64 Address = reinterpret_cast<int64>(InPtr);	// 페이지 가드를 넘어 실제로 사용하는 공간의 시작 주소
	const int64 BaseAddress = Address - (Address % PageSize);	// 할당받은 전체 공간의 시작 주소

	::VirtualFree(reinterpret_cast<void*>(BaseAddress), 0, MEM_RELEASE);
}
