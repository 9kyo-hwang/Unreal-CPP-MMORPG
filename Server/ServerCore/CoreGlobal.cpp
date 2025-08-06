#include "pch.h"
#include "CoreGlobal.h"

#include "GlobalAsyncTaskManager.h"
#include "MemoryBase.h"
#include "Profiler.h"
#include "SocketSubsystem.h"
#include "ThreadManager.h"
#include "SendBuffer.h"

FThreadManager* GThreadManager = nullptr;
FMemory* GMemory = nullptr;
FSendBufferPool* GSendBufferPool = nullptr;
FAsyncTaskQueueManager* GAsyncTaskQueueManager = nullptr;
FDeadLockProfiler* GDeadLockProfiler = nullptr;

// 매니저 간 호출 순서를 조정해주는 역할
class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new FThreadManager();
		GMemory = new FMemory();
		GSendBufferPool = new FSendBufferPool();
		GAsyncTaskQueueManager = new FAsyncTaskQueueManager();
		GDeadLockProfiler = new FDeadLockProfiler();
		FSocketSubsystem::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemory;
		delete GSendBufferPool;
		delete GAsyncTaskQueueManager;
		delete GDeadLockProfiler;
		FSocketSubsystem::Shutdown();
	}
} GCoreGlobal;