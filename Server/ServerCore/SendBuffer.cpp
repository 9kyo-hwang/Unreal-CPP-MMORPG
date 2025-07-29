#include "pch.h"
#include "SendBuffer.h"

FSendBuffer::FSendBuffer(shared_ptr<FSendBufferChunk> InOwner, BYTE* InData, uint32 InCapacity)
	: Owner(InOwner)
	, Data(InData)
	, UsedSize(0)
	, Capacity(InCapacity)
{
}

FSendBuffer::~FSendBuffer()
{
}

void FSendBuffer::Close(uint32 InUsedSize)
{
	check(Capacity >= InUsedSize);
	UsedSize = InUsedSize;
	Owner->Close(InUsedSize);
}

FSendBufferChunk::FSendBufferChunk()
	: Buffer()
	, bIsOpen(false)
	, UsedSize(0)
{
}

FSendBufferChunk::~FSendBufferChunk()
{
}

void FSendBufferChunk::Clear()
{
	bIsOpen = false;
	UsedSize = 0;
}

shared_ptr<FSendBuffer> FSendBufferChunk::Open(uint32 Capacity)
{
	check(ChunkSize >= Capacity);
	check(bIsOpen == false);

	if (Capacity > GetFreeSize())
	{
		return nullptr;
	}

	bIsOpen = true;
	return TObjectPool<FSendBuffer>::MakeShared(AsShared(), GetData(), Capacity);
}

void FSendBufferChunk::Close(uint32 InUsedSize)
{
	check(bIsOpen == true);
	bIsOpen = false;
	UsedSize += InUsedSize;
}

shared_ptr<FSendBuffer> FSendBufferPool::Open(int32 Size)
{
	// 스레드마다 들고 있는 변수여서 경합이 일어나지 않음
	if (!LSendBufferChunk)
	{
		LSendBufferChunk = Get(); // WRITE_LOCK
		LSendBufferChunk->Clear();
	}

	check(LSendBufferChunk->IsOpen() == false);

	// 다 썼으면 버리고 새거로 교체
	if (LSendBufferChunk->GetFreeSize() < Size)
	{
		LSendBufferChunk = Get();
		LSendBufferChunk->Clear();
	}

	cout << "FREE: " << LSendBufferChunk->GetFreeSize() << endl;

	return LSendBufferChunk->Open(Size);
}

shared_ptr<FSendBufferChunk> FSendBufferPool::Get()
{
	printf("FSendBufferPool::Get()\n");
	{
		WRITE_LOCK;
		if (!Chunks.empty())
		{
			auto Chunk = Chunks.top();
			Chunks.pop();
			return Chunk;
		}
	}

	// 레퍼런스 카운트가 0이 되면 Delete<>()를 호출하지 않고 다시 풀에 반환하도록 설정
	return shared_ptr<FSendBufferChunk>(New<FSendBufferChunk>(), OnDelete);
}

void FSendBufferPool::Return(shared_ptr<FSendBufferChunk> Chunk)
{
	WRITE_LOCK;
	Chunks.push(Chunk);
}

void FSendBufferPool::OnDelete(FSendBufferChunk* Chunk)
{
	printf("FSendBufferPool::OnDelete()\n");
	GSendBufferPool->Return(shared_ptr<FSendBufferChunk>(Chunk, OnDelete));
}
