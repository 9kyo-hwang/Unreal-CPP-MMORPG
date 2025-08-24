#include "BufferReader.h"

/*----------------
	FBufferReader
-----------------*/

BufferReader::BufferReader()
{
}

BufferReader::BufferReader(BYTE* InBuffer, uint32 InSize, uint32 InPos)
	: Buffer(InBuffer), Size(InSize), Pos(InPos)
{

}

BufferReader::~BufferReader()
{

}

bool BufferReader::Peek(void* Dest, uint32 Length) const
{
	if (GetFreeSize() < Length)
	{
		return false;
	}

	::memcpy(Dest, &Buffer[Pos], Length);
	return true;
}

bool BufferReader::Read(void* Dest, uint32 Length)
{
	if (Peek(Dest, Length) == false)
	{
		return false;
	}

	Pos += Length;
	return true;
}
