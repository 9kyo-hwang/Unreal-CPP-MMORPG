#include "pch.h"

#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "Service.h"
#include "ThreadManager.h"
#include "Protocol.pb.h"
#include <functional>

#include "GameMode.h"

int main()
{
	ClientPacketHandler::Initialize();

	auto Service = MakeShared<FServerService>(
		FInternetAddr(TEXT("127.0.0.1"), 7777),
		MakeShared<FSocketEventQueue>(),
		MakeShared<FClientSession>,	// ()를 붙이면 안됨. 추후 SessionManager 등에서 관리
		100
	);

	check(Service->Run());

	// 보통 스레드 개수는 코어 개수 ~ 코어 개수 * 1.5
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->AddThread([=]()
			{
				while (true)
				{
					Service->GetEventQueue()->Dequeue();
				}
			});
	}

	while (true)
	{
		GGameMode->Flush();
		this_thread::sleep_for(1s);
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
