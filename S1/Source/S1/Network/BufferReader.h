#pragma once

/*----------------
	FBufferReader
-----------------*/

class BufferReader
{
public:
	BufferReader();
	BufferReader(BYTE* InBuffer, uint32 InSize, uint32 InPos = 0);
	~BufferReader();

	BYTE*			GetBuffer() const { return Buffer; }
	uint32			GetSize() const { return Size; }
	uint32			GetReadSize() const { return Pos; }
	uint32			GetFreeSize() const { return Size - Pos; }

	template<typename T>
	bool			Peek(T* Dest) { return Peek(Dest, sizeof(T)); }
	bool			Peek(void* Dest, uint32 Length) const;

	template<typename T>
	bool			Read(T* Dest) { return Read(Dest, sizeof(T)); }
	bool			Read(void* Dest, uint32 Length);

	template<typename T>
	BufferReader&	operator>>(OUT T& Dest);

private:
	BYTE*			Buffer = nullptr;
	uint32			Size = 0;
	uint32			Pos = 0;
};

template<typename T>
inline BufferReader& BufferReader::operator>>(OUT T& Dest)
{
	Dest = *reinterpret_cast<T*>(&Buffer[Pos]);
	Pos += sizeof(T);
	return *this;
}