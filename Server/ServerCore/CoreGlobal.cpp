#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

FThreadManager* GThreadManager = nullptr;

// 매니저 간 호출 순서를 조정해주는 역할
class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new FThreadManager();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
	}
} GCoreGlobal;