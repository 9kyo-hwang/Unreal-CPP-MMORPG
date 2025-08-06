#include "pch.h"
#include "CoreTLS.h"

thread_local uint32 LThreadID = 0;
thread_local uint64 LEndTick = 0;
thread_local TStack<int32> LLockStack;
thread_local shared_ptr<FSendBufferChunk> LSendBufferChunk = nullptr;
thread_local FAsyncTaskQueue* LAsyncTaskQueue = nullptr;