#include "pch.h"
#include <thread>
#include "CoreGlobal.h"
#include "Memory.h"
#include "RefCountBase.h"
#include "ThreadManager.h"

class Knight
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}
	Knight(int32 InHp)
	{
		cout << "Knight(int32)" << endl;
	}
	~Knight()
	{
		cout << "~Knight()" << endl;
	}
};

int main()
{
	Knight* knight = NewObject<Knight>(100);
	DeleteObject(knight);

	return 0;
}
