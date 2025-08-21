#include "pch.h"
#include "CoreGlobal.h"
#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"
#include "JobTimer.h"

FThreadManager* GThreadManager = nullptr;
FGlobalJobQueue* GGlobalQueue = nullptr;
FJobTimer* GJobTimer = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new FThreadManager();
		GGlobalQueue = new FGlobalJobQueue();
		GJobTimer = new FJobTimer();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GGlobalQueue;
		delete GJobTimer;
		SocketUtils::Clear();
	}
} GCoreGlobal;