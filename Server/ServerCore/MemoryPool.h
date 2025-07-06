#pragma once

/**
 *	같은 크기를 가지는 인스턴스끼리 Pool에 묶어 줌
 *	그리고 그러한 Pool들을 여러 개 들고 있도록 함
 *	예) 32byte Pool, 64byte Pool, ...
 *	이를 위해, 동적 할당 시 메타 데이터를 들고 있도록 설계(실제로 C++ 표준에서 동적할당 시 객체 크기와 다음 Heap 메모리 주소 등을 Header로 들고 있음
 */

struct FAllocationData
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

class FMemoryPool
{
public:
	FMemoryPool(int32 InSize);
	~FMemoryPool();

	void Push(FAllocationData* InAllocDataPtr);
	FAllocationData* Pop();

private:
	int32 Size;	// 해당 Pool이 담당하는 할당 크기
	TAtomic<int32> Num;	// 현재 할당한 메모리 영역 개수

	USE_LOCK;
	queue<FAllocationData*> Queue;
};

