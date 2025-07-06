#include "pch.h"
#include "CoreGlobal.h"

#include "MemoryBase.h"
#include "Profiler.h"
#include "ThreadManager.h"

FThreadManager* GThreadManager = nullptr;
FMemory* GMemory = nullptr;
FDeadLockProfiler* GDeadLockProfiler = nullptr;

// 매니저 간 호출 순서를 조정해주는 역할
class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new FThreadManager();
		GMemory = new FMemory();
		GDeadLockProfiler = new FDeadLockProfiler();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemory;
		delete GDeadLockProfiler;
	}
} GCoreGlobal;