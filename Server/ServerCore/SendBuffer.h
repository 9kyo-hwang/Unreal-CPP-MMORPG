#pragma once

class FSendBufferChunk;

class FSendBuffer : enable_shared_from_this<FSendBuffer>
{
public:
	FSendBuffer(uint32 InCapacity);
	~FSendBuffer();

	BYTE* GetData() { return Data.data(); }
	uint32 GetUsedSize() const { return UsedSize; }
	uint32 GetCapacity() const { return Capacity; }

	void CopyData(void* InData, int32 Size);
	void Close(uint32 InUsedSize);

private:
	vector<BYTE> Data;
	uint32 UsedSize;	// 사용 중인 크기
	uint32 Capacity;	// 최대 크기
};