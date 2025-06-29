#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

FThreadManager* GThreadManager = nullptr;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new FThreadManager();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
}
