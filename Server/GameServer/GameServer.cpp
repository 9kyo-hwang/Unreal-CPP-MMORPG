#include "pch.h"

#include "GameSession.h"
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

	BYTE SendData[1024] = "Hello, World!";
	while (true)
	{
		shared_ptr<FSendBuffer> SendBuffer = GSendBufferPool->Open(4096);
		BYTE* Buffer = SendBuffer->GetData();
		reinterpret_cast<FPacketHeader*>(Buffer)->Size = sizeof(SendData) + sizeof(FPacketHeader);
		reinterpret_cast<FPacketHeader*>(Buffer)->Id = 1;
		::memcpy(&Buffer[4], SendData, sizeof(SendData));
		SendBuffer->Close(sizeof(SendData) + sizeof(FPacketHeader));

		GSessionManager.Broadcast(SendBuffer); // Broadcast to all sessions

		this_thread::sleep_for(250ms);
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
