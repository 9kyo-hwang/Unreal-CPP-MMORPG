#include "pch.h"
#include "SendBuffer.h"

FSendBuffer::FSendBuffer(uint32 InCapacity)
	: UsedSize(0)
	, Capacity(InCapacity)
{
	Data.resize(InCapacity);
}

FSendBuffer::~FSendBuffer()
{
}

void FSendBuffer::CopyData(void* InData, int32 Size)
{
	check(GetCapacity() >= Size);
	::memcpy(Data.data(), InData, Size);
	UsedSize = Size;
}

void FSendBuffer::Close(uint32 InUsedSize)
{
	check(Capacity >= InUsedSize);
	UsedSize = InUsedSize;
}
