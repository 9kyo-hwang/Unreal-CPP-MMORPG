#include "pch.h"

#include "IPAddress.h"
#include "Listener.h"
#include "ThreadManager.h"

int main()
{
	FListener Listener;
	Listener.Run({ TEXT("127.0.0.1"), 7777 });

	// 보통 스레드 개수는 코어 개수 ~ 코어 개수 * 1.5
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->AddThread([=]()
			{
				while (true)
				{
					GCompletionPort.Dequeue();
				}
			});
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
