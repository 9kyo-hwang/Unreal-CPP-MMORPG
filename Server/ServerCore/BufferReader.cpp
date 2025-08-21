#include "pch.h"
#include "BufferReader.h"

/*----------------
	FBufferReader
-----------------*/

FBufferReader::FBufferReader()
{
}

FBufferReader::FBufferReader(BYTE* InBuffer, uint32 InSize, uint32 InPos)
	: Buffer(InBuffer), Size(InSize), Pos(InPos)
{

}

FBufferReader::~FBufferReader()
{

}

bool FBufferReader::Peek(void* Dest, uint32 Length) const
{
	if (GetFreeSize() < Length)
	{
		return false;
	}

	::memcpy(Dest, &Buffer[Pos], Length);
	return true;
}

bool FBufferReader::Read(void* Dest, uint32 Length)
{
	if (Peek(Dest, Length) == false)
	{
		return false;
	}

	Pos += Length;
	return true;
}
