#include "pch.h"
#include "CoreGlobal.h"
#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "TaskDispatcher.h"
#include "TaskScheduler.h"

FThreadManager* GThreadManager = nullptr;
FTaskQueueDispatcher* GTaskQueueDispatcher = nullptr;
FTaskScheduler* GTaskScheduler = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new FThreadManager();
		GTaskQueueDispatcher = new FTaskQueueDispatcher();
		GTaskScheduler = new FTaskScheduler();
		FSocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GTaskQueueDispatcher;
		delete GTaskScheduler;
		FSocketUtils::Clear();
	}
} GCoreGlobal;