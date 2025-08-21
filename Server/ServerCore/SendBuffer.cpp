#include "pch.h"
#include "SendBuffer.h"

/*----------------
	FSendBuffer
-----------------*/

FSendBuffer::FSendBuffer(int32 InBufferSize)
{
	Buffer.resize(InBufferSize);
}

FSendBuffer::~FSendBuffer()
{
}

void FSendBuffer::CopyData(void* InData, int32 InLength)
{
	assert(GetCapacity() >= InLength);
	::memcpy(Buffer.data(), InData, InLength);
	WriteSize = InLength;
}

void FSendBuffer::Close(uint32 InWriteSize)
{
	WriteSize = InWriteSize;
}
