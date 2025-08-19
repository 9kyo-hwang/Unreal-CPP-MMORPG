#pragma once

extern thread_local uint32 LThreadID;
extern thread_local uint64 LEndTick;
extern thread_local class FAsyncTaskQueue* LAsyncTaskQueue;