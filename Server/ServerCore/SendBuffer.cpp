#include "pch.h"
#include "SendBuffer.h"

FSendBuffer::FSendBuffer(int32 Size)
	: WriteSize(0)
{
	Buffer.resize(Size);
}

FSendBuffer::~FSendBuffer()
{
}

void FSendBuffer::PushData(void* InData, int32 InSize)
{
	check(Max() >= InSize);
	::memcpy(Buffer.data(), InData, InSize);

	WriteSize = InSize;
}
