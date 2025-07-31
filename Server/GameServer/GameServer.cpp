#include "pch.h"

#include "BufferWriter.h"
#include "GameSession.h"
#include "ServerPacketHandler.h"
#include "Service.h"
#include "SessionManager.h"
#include "ThreadManager.h"

int main()
{
	auto Service = MakeShared<FServerService>(
		FInternetAddr(TEXT("127.0.0.1"), 7777),
		MakeShared<FSocketEventQueue>(),
		MakeShared<FGameSession>,	// ()를 붙이면 안됨. 추후 SessionManager 등에서 관리
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
		TArray<FBuffData> Buffs{ {100, 1.5f}, {200, 2.3f}, {300, 0.7f} };
		shared_ptr<FSendBuffer> SendBuffer = ServerPacketHandler::CreatePacket_Test(1001, 100, 10, Buffs, TEXT("안녕하세요"));
		GSessionManager.Broadcast(SendBuffer); // Broadcast to all sessions

		this_thread::sleep_for(250ms);
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
