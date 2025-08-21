#include "pch.h"
#include "RecvBuffer.h"

/*--------------
	FReceiveBuffer
----------------*/

FReceiveBuffer::FReceiveBuffer(int32 InBufferSize) : BufferSize(InBufferSize)
{
	Capacity = InBufferSize * BUFFER_COUNT;
	Buffer.resize(Capacity);
}

FReceiveBuffer::~FReceiveBuffer()
{
}

void FReceiveBuffer::Clear()
{
	int32 DataSize = GetDataSize();
	if (DataSize == 0)
	{
		// 딱 마침 읽기+쓰기 커서가 동일한 위치라면, 둘 다 리셋.
		ReadPosition = WritePosition = 0;
	}
	else
	{
		// 여유 공간이 버퍼 1개 크기 미만이면, 데이터를 앞으로 땅긴다.
		if (GetFreeSize() < BufferSize)
		{
			::memcpy(&Buffer[0], &Buffer[ReadPosition], DataSize);
			ReadPosition = 0;
			WritePosition = DataSize;
		}
	}
}

bool FReceiveBuffer::OnRead(int32 NumOfBytes)
{
	if (NumOfBytes > GetDataSize())
	{
		return false;
	}

	ReadPosition += NumOfBytes;
	return true;
}

bool FReceiveBuffer::OnWrite(int32 NumOfBytes)
{
	if (NumOfBytes > GetFreeSize())
	{
		return false;
	}

	WritePosition += NumOfBytes;
	return true;
}