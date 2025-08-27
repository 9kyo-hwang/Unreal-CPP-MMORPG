#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
//#include "ClientPacketHandler.h"
#include <tchar.h>
#include "Job.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(FServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetEventQueue()->Dispatch(10);

		// 예약된 일감 처리
		FThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		FThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ServerPacketHandler::Init();

	FServerServiceRef service = make_shared<FServerService>(
		FNetAddress(L"127.0.0.1", 7777),
		make_shared<FSocketIOEventQueue>(),
		[=]() { return make_shared<FGameSession>(); }, // TODO : SessionManager 등
		100);

	check(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// Main Thread
	//DoWorkerJob(service);

	while (true)
	{
		//Protocol::S_CHAT Packet;
		//Packet.set_msg("HelloWorld");
		//auto SendBuffer = ServerPacketHandler::MakeSendBuffer(Packet);

		//GSessionManager.Broadcast(SendBuffer);
		this_thread::sleep_for(1s);
	}

	GThreadManager->Join();
}