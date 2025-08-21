#pragma once

class SendBufferChunk;

/*----------------
	FSendBuffer
-----------------*/

class FSendBuffer : TSharedFromThis<FSendBuffer>
{
public:
	FSendBuffer(int32 InBufferSize);
	~FSendBuffer();

	BYTE* GetData() { return Buffer.data(); }
	int32 GetWriteSize() const { return WriteSize; }
	int32 GetCapacity() const { return static_cast<int32>(Buffer.size()); }

	void CopyData(void* InData, int32 InLength);
	void Close(uint32 InWriteSize);

private:
	vector<BYTE>	Buffer;
	int32			WriteSize = 0;
};

