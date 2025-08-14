#include "pch.h"

#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "Service.h"
#include "ThreadManager.h"
#include "Protocol.pb.h"
#include <functional>

#include "GameMode.h"
#include "DataBaseConnectionPool.h"
#include "DataBaseBinder.h"
#include "DatabaseSynchronizer.h"
#include "RapidXml.h"

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
	// Live Service이면 서버 주소, DB 이름, Account 정보 등을 다 별도의 파일로 관리
	check(GDataBaseConnectionPool->Open(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));

	FDatabaseConnection* Connection = GDataBaseConnectionPool->Get();
	FDatabaseSynchronizer Synchronizer(*Connection);
	Synchronizer.Synchronize(TEXT("GameDB.xml"));

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
		GThreadManager->AddThread([&Service]()
			{
				while (true)
				{
					WorkerThreadMain(Service);
				}
			});
	}

	WorkerThreadMain(Service);

	GThreadManager->WaitForCompletion();

	return 0;
}
