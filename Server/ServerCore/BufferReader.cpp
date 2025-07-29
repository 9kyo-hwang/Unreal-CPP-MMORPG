#include "pch.h"
#include "BufferReader.h"

FBufferReader::FBufferReader()
	: ReaderData(nullptr)
	, ReaderPos(0)
	, ReaderSize(0)
{
}

FBufferReader::FBufferReader(BYTE* Data, uint32 Size, uint32 Pos)
	: ReaderData(Data)
	, ReaderPos(Pos)
	, ReaderSize(Size)
{
}

FBufferReader::~FBufferReader()
{
}

bool FBufferReader::Peek(void* Dest, uint32 InSize)
{
	if (GetFreeSize() < InSize)
	{
		return false;
	}

	::memcpy(Dest, &ReaderData[ReaderPos], InSize);
	return true;
}

bool FBufferReader::Read(void* Dest, uint32 InSize)
{
	if (!Peek(Dest, InSize))
	{
		return false;
	}

	ReaderPos += InSize;
	return true;
}
