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

int main()
{
	for (int32 i = 0; i < 3; ++i)
	{
		GThreadManager->AddThread([]()
			{
				while (true)
				{
					Knight* K = New<Knight>();
					cout << K->Hp << endl;
					this_thread::sleep_for(10ms);
					Delete(K);
				}
			});
	}

	return 0;
}
