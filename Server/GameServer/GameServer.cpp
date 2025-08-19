#include "pch.h"

#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "Service.h"
#include "ThreadManager.h"
#include "Protocol.pb.h"
#include <functional>

static constexpr uint64 WorkerTimeoutTick = 64;

void WorkerThreadMain(shared_ptr<FServerService>& Service)
{
	while (true)
	{
		LEndTick = ::GetTickCount64() + WorkerTimeoutTick;

		// Network IO + InGame Logic(by Packet Handler)
		Service->GetEventQueue()->Dequeue(10);

		// 예약된 Task 처리
		FThreadManager::DistributeReservedTasks();

		// 작업 처리 Tick이 남았다면, Global AsyncTaskQueue도 처리해버림
		FThreadManager::QueueAsyncTask();
	}
}

int main()
{
	ClientPacketHandler::Initialize();

	auto Service = make_shared<FServerService>(
		FInternetAddr(TEXT("127.0.0.1"), 7777),
		make_shared<FSocketEventQueue>(),
		[=]() { return make_shared<FClientSession>(); },	// ()를 붙이면 안됨. 추후 SessionManager 등에서 관리
		100
	);

	check(Service->Run());

	// 보통 스레드 개수는 코어 개수 ~ 코어 개수 * 1.5
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->AddThread([&Service]()
			{
				WorkerThreadMain(Service);
			});
	}

	WorkerThreadMain(Service);

	GThreadManager->WaitForCompletion();

	return 0;
}
