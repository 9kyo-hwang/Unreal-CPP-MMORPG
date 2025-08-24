#include "BufferWriter.h"

/*----------------
	BufferWriter
-----------------*/

BufferWriter::BufferWriter()
{

}

BufferWriter::BufferWriter(BYTE* InBuffer, uint32 InSize, uint32 InPos)
	: Buffer(InBuffer), Size(InSize), Pos(InPos)
{

}

BufferWriter::~BufferWriter()
{

}

bool BufferWriter::Write(void* Src, uint32 Length)
{
	if (GetFreeSize() < Length)
		return false;

	::memcpy(&Buffer[Pos], Src, Length);
	Pos += Length;
	return true;
}