#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

FThreadManager* GThreadManager = nullptr;

// �Ŵ��� �� ȣ�� ������ �������ִ� ����
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