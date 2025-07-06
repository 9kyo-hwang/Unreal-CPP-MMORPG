#include "pch.h"
#include <thread>
#include "CoreGlobal.h"
#include "Allocator.h"
#include "Memory.h"
#include "RefCountBase.h"
#include "ThreadManager.h"

class Knight
{
public:
	Knight() { cout << "Knight()\n"; }
	Knight(int32 hp) { cout << "Knight(int32)\n"; }
	~Knight() { cout << "~Knight()\n"; }

	int32 _hp = 0;
	int32 _mp = 0;
};

int main()
{
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->AddThread([]()
			{
				while (true)
				{
					TArray<Knight> Knights(10);
					TSortedMap<int32, Knight> Map;
					Map[100] = Knight();

					this_thread::sleep_for(10ms);
				}
			});
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
