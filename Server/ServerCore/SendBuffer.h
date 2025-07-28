#pragma once

class FSendBuffer
{
public:
	FSendBuffer(shared_ptr<FSendBufferChunk> InOwner, BYTE* InData, int32 InCapacity);
	~FSendBuffer();

	BYTE* GetData() const { return Data; }
	uint32 GetUsedSize() const { return UsedSize; }
	void Close(int32 InUsedSize);

private:
	shared_ptr<FSendBufferChunk> Owner;	// 버퍼 공간을 제공한 청크
	BYTE* Data;			// 직접 영역을 관리하는 것이 아닌, 청크로부터 얻은 버퍼 시작 주소
	uint32 UsedSize;	// 사용 중인 크기
	uint32 Capacity;	// 최대 크기
};

// 버퍼 풀에서 스레드 변수 청크를 사용하기 때문에, 사실상 싱글 스레드 환경에서 동작
class FSendBufferChunk : public TSharedFromThis<FSendBufferChunk>
{
	static constexpr int32 ChunkSize = 0x2000; // 8KB

public:
	FSendBufferChunk();
	~FSendBufferChunk();

	void Clear();
	shared_ptr<FSendBuffer> Open(uint32 Capacity);	// 할당할 공간 크기
	void Close(uint32 InUsedSize);	// 실제 사용한 사이즈

	bool IsOpen() const { return bIsOpen; }	// Open 후 Close 해야만 다시 Open할 수 있음
	BYTE* GetData() { return &Buffer[UsedSize]; }	// 버퍼의 시작점이 아닌, 사용하고 있는 공간의 끝 위치
	uint32 GetFreeSize() const { return Buffer.size() - UsedSize; }	// 남은 공간 크기

private:
	TStaticArray<uint8, ChunkSize> Buffer;
	bool bIsOpen;
	uint32 UsedSize;
};

class FSendBufferPool
{
public:
	shared_ptr<FSendBuffer> Open(int32 Size);

private:
	shared_ptr<FSendBufferChunk> Get();
	void Return(shared_ptr<FSendBufferChunk> Chunk);

	static void OnDelete(FSendBufferChunk* Chunk);

private:
	USE_LOCK;
	TStack<shared_ptr<FSendBufferChunk>> Chunks;
};