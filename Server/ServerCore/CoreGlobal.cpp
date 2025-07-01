#include "pch.h"
#include "CoreGlobal.h"

#include "Profiler.h"
#include "ThreadManager.h"

FThreadManager* GThreadManager = nullptr;
FDeadLockProfiler* GDeadLockProfiler = nullptr;

// 매니저 간 호출 순서를 조정해주는 역할
class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new FThreadManager();
		GDeadLockProfiler = new FDeadLockProfiler();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GDeadLockProfiler;
	}
} GCoreGlobal;