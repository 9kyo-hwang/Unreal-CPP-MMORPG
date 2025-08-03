#include "pch.h"

#include "BufferWriter.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "Service.h"
#include "SessionManager.h"
#include "ThreadManager.h"
#include "Protocol.pb.h"

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

	GThreadManager->WaitForCompletion();

	return 0;
}
