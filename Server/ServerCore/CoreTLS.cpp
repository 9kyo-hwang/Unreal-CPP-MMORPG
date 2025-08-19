#include "pch.h"
#include "CoreTLS.h"

thread_local uint32 LThreadID = 0;
thread_local uint64 LEndTick = 0;
thread_local FAsyncTaskQueue* LAsyncTaskQueue = nullptr;