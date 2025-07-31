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

	template<typename T> FBufferWriter& operator<<(T&& Src);
	template<typename T> T* Reserve();

private:
	BYTE* WriterData;
	uint32 WriterPos;
	uint32 WriterSize;
};

/** 템플릿이 붙는 순간 오른값 참조가 아닌 보편 참조 -> 다른 형태를 전부 무시하고 무조건 해당 호출로 귀결됨
 *	- 왼값이 들어오면 const T&로, 오른값이 들어오면 T&&로 처리
 *	- 우리의 경우 uint64 등을 넘겨주고 있는데 이 경우 *reinterpret_cast<const uint64&*>를 시도
 *	- 따라서 참조가 붙은 채 넘어왔으면 이를 떼주는 작업 수행
 */

template <typename T>
FBufferWriter& FBufferWriter::operator<<(T&& Src)
{
	using ValueType = std::remove_reference_t<T>;

	*reinterpret_cast<ValueType*>(&WriterData[WriterPos]) = std::forward<ValueType>(Src);
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
