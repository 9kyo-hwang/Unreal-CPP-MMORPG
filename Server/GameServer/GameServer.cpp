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
	TMap<int32, Knight> Map;
	Map[100] = Knight();

	return 0;
}
