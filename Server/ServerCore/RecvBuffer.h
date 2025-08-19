#pragma once

class FRecvBuffer
{
public:
	FRecvBuffer(int32 InBufferSize);
	~FRecvBuffer();

	void Clear();
	bool AdvanceReadPosition(int32 Size);
	bool AdvanceWritePosition(int32 Size);

	BYTE* GetReadPosition() { return &Buffer[ReadPos]; }
	BYTE* GetWritePosition() { return &Buffer[WritePos]; }
	int32 GetDataSize() const { return WritePos - ReadPos; }
	int32 GetFreeSize() const { return Capacity - WritePos; }

private:
	int32 Capacity;
	int32 BufferSize;
	int32 ReadPos;
	int32 WritePos;
	vector<uint8> Buffer;
};

