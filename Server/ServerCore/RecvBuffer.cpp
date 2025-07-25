#include "pch.h"
#include "RecvBuffer.h"

int32 BufferCount = 10;

FRecvBuffer::FRecvBuffer(int32 InBufferSize)
	: Capacity(InBufferSize * BufferCount)	// 버퍼를 여러 개 들고 있는 것처럼 공간을 크게 할당
	, BufferSize(InBufferSize)	// 버퍼 1개의 크기
	, ReadPos(0)
	, WritePos(0)
{
	Buffer.resize(Capacity);
}

FRecvBuffer::~FRecvBuffer()
{
}

void FRecvBuffer::Clear()
{
	// 들고 있는 데이터가 없으면(write-read == 0) 커서를 0 위치로 이동
	int32 DataSize = GetDataSize();
	if (DataSize == 0)
	{
		ReadPos = WritePos = 0;
		return;
	}

	// 여유 공간이 버퍼 1개 크기보다 작으면 남은 데이터를 앞으로 이동
	if (GetFreeSize() < BufferSize)
	{
		::memcpy(&Buffer[0], &Buffer[ReadPos], DataSize);
		ReadPos = 0;
		WritePos = DataSize;
	}
}

bool FRecvBuffer::AdvanceReadPosition(int32 Size)
{
	// 데이터 크기보다 더 많이 읽으려고 하면 false
	if (Size > GetDataSize())
	{
		return false;
	}

	ReadPos += Size;
	return true;
}

bool FRecvBuffer::AdvanceWritePosition(int32 Size)
{
	// 빈 공간보다 더 많이 쓰려고 하면 false
	if (Size > GetFreeSize())
	{
		return false;
	}

	WritePos += Size;
	return true;
}
