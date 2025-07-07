#include "pch.h"
#include <thread>
#include "CoreGlobal.h"
#include "Allocator.h"
#include "Memory.h"
#include "MemoryBase.h"
#include "RefCountBase.h"
#include "ThreadManager.h"

class Knight
{
public:
	int32 Hp = rand() % 1000;
};

class Monster
{
public:
	int64 Id = 0;
};

int main()
{
	Knight* Knights[100];
	for (int32 i = 0; i < 100; ++i)
	{
		Knights[i] = TObjectPool<Knight>::Get();
	}

	for (int32 i = 0; i < 100; ++i)
	{
		TObjectPool<Knight>::Release(Knights[i]);
		Knights[i] = nullptr;
	}

	shared_ptr<Knight> K = TObjectPool<Knight>::MakeShared();
	shared_ptr<Knight> K2 = MakeShared<Knight>();

	return 0;
}
