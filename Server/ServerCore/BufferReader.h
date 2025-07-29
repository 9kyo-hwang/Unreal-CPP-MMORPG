#pragma once

class FBufferReader
{
public:
	FBufferReader();
	FBufferReader(BYTE* Data, uint32 Size, uint32 Pos = 0);
	~FBufferReader();

	BYTE* GetReaderData() const { return ReaderData; }
	uint32 GetReadSize() const { return ReaderPos; }
	uint32 GetFreeSize() const { return ReaderSize - ReaderPos; }

	template<typename T> bool Peek(T* Dest) { return Peek(Dest, sizeof(T)); }
	bool Peek(void* Dest, uint32 InSize);

	template<typename T> bool Read(T* Dest) { return Read(Dest, sizeof(T)); }
	bool Read(void* Dest, uint32 InSize);

	template<typename T> FBufferReader& operator>>(T& Dest);

private:
	BYTE* ReaderData;
	uint32 ReaderPos;
	uint32 ReaderSize;
};

template <typename T>
FBufferReader& FBufferReader::operator>>(T& Dest)
{
	Dest = *reinterpret_cast<T*>(&ReaderData[ReaderPos]);
	ReaderPos += sizeof(T);
	return *this;
}

