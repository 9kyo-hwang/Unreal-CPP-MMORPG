#pragma once

/*--------------
	FReceiveBuffer
----------------*/

class FReceiveBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	FReceiveBuffer(int32 InBufferSize);
	~FReceiveBuffer();

	void			Clear();
	bool			OnRead(int32 NumOfBytes);
	bool			OnWrite(int32 NumOfBytes);

	BYTE*			ReadPos() { return &Buffer[ReadPosition]; }
	BYTE*			WritePos() { return &Buffer[WritePosition]; }
	int32			GetDataSize() const { return WritePosition - ReadPosition; }
	int32			GetFreeSize() const { return Capacity - WritePosition; }

private:
	int32			Capacity = 0;
	int32			BufferSize = 0;
	int32			ReadPosition = 0;
	int32			WritePosition = 0;
	vector<BYTE>	Buffer;
};

