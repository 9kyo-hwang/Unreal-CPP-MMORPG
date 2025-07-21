#pragma once

constexpr uint64 Alignment = 16;
using FListEntry = SLIST_ENTRY;
using FListHeader = SLIST_HEADER;

/**
 *	같은 크기를 가지는 인스턴스끼리 Pool에 묶어 줌
 *	그리고 그러한 Pool들을 여러 개 들고 있도록 함
 *	예) 32byte Pool, 64byte Pool, ...
 *	이를 위해, 동적 할당 시 메타 데이터를 들고 있도록 설계(실제로 C++ 표준에서 동적할당 시 객체 크기와 다음 Heap 메모리 주소 등을 Header로 들고 있음
 */

__declspec(align(Alignment))
struct FAllocationData : public FListEntry	// 메모리 구조 최상단에 두기 위해서 상속
{
	FAllocationData(int32 InSize)
		: Size(InSize)
	{}

	static void* Attach(FAllocationData* InData, int32 InSize)
	{
		new(InData)FAllocationData(InSize);
		return ++InData;	// int32 하나 들고 있기 때문에, 해당 주소에서 ++해주면 Size 값을 건너뛴 실제 데이터 시작 주소
	}

	static FAllocationData* Detach(void* InPtr)
	{
		FAllocationData* Data = static_cast<FAllocationData*>(InPtr) - 1;
		return Data;
	}

	SIZE_T Size;
};

__declspec(align(Alignment))
class FMemoryPool
{
public:
	FMemoryPool(int32 InSize);
	~FMemoryPool();

	void Release(FAllocationData* InAllocDataPtr);
	FAllocationData* Get();

private:
	FListHeader ListHead;	// Lock-Free Stack의 Head
	int32 MallocSize;	// 해당 Pool이 담당하는 할당 크기
	TAtomic<int32> NumUses;		// 현재 풀에서 사용 중인 개수
	TAtomic<int32> NumReserved;	// 현재 풀에 저장된 개수
};
