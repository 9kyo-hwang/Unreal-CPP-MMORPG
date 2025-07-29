#pragma once

class FBufferWriter
{
public:
	FBufferWriter();
	FBufferWriter(BYTE* Data, uint32 Size, uint32 Pos = 0);
	~FBufferWriter();

	BYTE* GetWriterData() const { return WriterData; }
	uint32 GetWriteSize() const { return WriterPos; }
	uint32 GetFreeSize() const { return WriterSize - WriterPos; }

	template<typename T> bool Write(T* Src) { return Write(Src, sizeof(T)); }
	bool Write(void* Src, uint32 InSize);

	template<typename T> FBufferWriter& operator<<(const T& Src);
	template<typename T> FBufferWriter& operator<<(T&& Src);
	template<typename T> T* Reserve();

private:
	BYTE* WriterData;
	uint32 WriterPos;
	uint32 WriterSize;
};

template <typename T>
FBufferWriter& FBufferWriter::operator<<(const T& Src)
{
	// Memcpy와 동일한 역할이나, 작은 데이터의 경우 이 코드가 더 성능이 좋음
	*reinterpret_cast<T*>(&WriterData[WriterPos]) = Src;
	WriterPos += sizeof(T);
	return *this;
}

template <typename T>
FBufferWriter& FBufferWriter::operator<<(T&& Src)
{
	*reinterpret_cast<T*>(&WriterData[WriterPos]) = move(Src);
	WriterPos += sizeof(T);
	return *this;
}

template <typename T>
T* FBufferWriter::Reserve()
{
	if (GetFreeSize() < sizeof(T))
	{
		return nullptr;
	}

	T* Ptr = reinterpret_cast<T*>(&WriterData[WriterPos]);
	WriterPos += sizeof(T);
	return Ptr;
}
