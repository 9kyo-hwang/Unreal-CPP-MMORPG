#include "pch.h"
#include "CoreGlobal.h"

#include "Profiler.h"
#include "ThreadManager.h"

FThreadManager* GThreadManager = nullptr;
FDeadLockProfiler* GDeadLockProfiler = nullptr;

// �Ŵ��� �� ȣ�� ������ �������ִ� ����
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