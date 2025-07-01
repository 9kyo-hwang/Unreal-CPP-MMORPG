#include "pch.h"
#include <thread>

#include "AccountManager.h"
#include "CoreGlobal.h"
#include "PlayerManager.h"
#include "ThreadManager.h"

int main()
{
	GThreadManager->AddThread([=]()
	{
		while (true)
		{
			cout << "Player Then Account" << endl;
			GPlayerManager.PlayerThenAccount();
			this_thread::sleep_for(100ms);
		}
	});

	GThreadManager->AddThread([=]()
		{
			while (true)
			{
				cout << "Account Then Player" << endl;
				GAccountManager.AccountThenPlayer();
				this_thread::sleep_for(100ms);
			}
		});

	GThreadManager->WaitForCompletion();

	return 0;
}
