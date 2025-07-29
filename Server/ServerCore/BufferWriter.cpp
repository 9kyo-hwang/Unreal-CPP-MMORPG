#include "pch.h"
#include "BufferWriter.h"

FBufferWriter::FBufferWriter()
	: WriterData(nullptr)
	, WriterSize(0)
	, WriterPos(0)
{
}

FBufferWriter::FBufferWriter(BYTE* Data, uint32 Size, uint32 Pos)
	: WriterData(Data)
	, WriterSize(Size)
	, WriterPos(Pos)
{
}

FBufferWriter::~FBufferWriter()
{
}

bool FBufferWriter::Write(void* Src, uint32 InSize)
{
	if (GetFreeSize() < InSize)
	{
		return false;
	}

	::memcpy(&WriterData[WriterPos], Src, InSize);
	WriterPos += InSize;
	return true;
}
