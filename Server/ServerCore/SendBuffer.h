#pragma once

class FSendBuffer : public TSharedFromThis<FSendBuffer>
{
public:
	FSendBuffer(int32 Size);
	~FSendBuffer();

	BYTE* GetData() { return Buffer.data(); }
	int32 Num() const { return WriteSize; }
	int32 Max() const { return Buffer.size(); }

	void PushData(void* InData, int32 InSize);

private:
	TArray<uint8> Buffer;
	int32 WriteSize;
};

