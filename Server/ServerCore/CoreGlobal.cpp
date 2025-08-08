#include "pch.h"
#include "CoreGlobal.h"

#include "AsyncTaskQueueManager.h"
#include "DataBaseConnectionPool.h"
#include "MemoryBase.h"
#include "Profiler.h"
#include "SocketSubsystem.h"
#include "ThreadManager.h"
#include "SendBuffer.h"
#include "TaskTimer.h"

FThreadManager* GThreadManager = nullptr;
FMemory* GMemory = nullptr;
FSendBufferPool* GSendBufferPool = nullptr;
FAsyncTaskQueueManager* GAsyncTaskQueueManager = nullptr;
FTaskTimerManager* GTaskTimerManager = nullptr;
FDeadLockProfiler* GDeadLockProfiler = nullptr;
FDataBaseConnectionPool* GDataBaseConnectionPool = nullptr;

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
		GTaskTimerManager = new FTaskTimerManager();
		GDeadLockProfiler = new FDeadLockProfiler();
		GDataBaseConnectionPool = new FDataBaseConnectionPool();
		FSocketSubsystem::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemory;
		delete GSendBufferPool;
		delete GAsyncTaskQueueManager;
		delete GTaskTimerManager;
		delete GDeadLockProfiler;
		delete GDataBaseConnectionPool;
		FSocketSubsystem::Shutdown();
	}
} GCoreGlobal;