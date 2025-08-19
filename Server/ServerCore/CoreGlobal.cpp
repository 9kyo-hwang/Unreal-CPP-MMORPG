#include "pch.h"
#include "CoreGlobal.h"

#include "AsyncTaskQueueManager.h"
#include "SocketSubsystem.h"
#include "ThreadManager.h"
#include "TaskTimer.h"

FThreadManager* GThreadManager = nullptr;
FAsyncTaskQueueManager* GAsyncTaskQueueManager = nullptr;
FTaskTimerManager* GTaskTimerManager = nullptr;

// 매니저 간 호출 순서를 조정해주는 역할
class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new FThreadManager();
		GAsyncTaskQueueManager = new FAsyncTaskQueueManager();
		GTaskTimerManager = new FTaskTimerManager();

		FSocketSubsystem::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GAsyncTaskQueueManager;
		delete GTaskTimerManager;

		FSocketSubsystem::Shutdown();
	}
} GCoreGlobal;