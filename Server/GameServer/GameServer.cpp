#include "pch.h"

#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "Service.h"
#include "ThreadManager.h"
#include "Protocol.pb.h"
#include <functional>

#include "GameMode.h"
#include "DataBaseConnectionPool.h"

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

	{
		auto Query = L"										\
				DROP TABLE IF EXISTS [dbo].[Gold];			\
				CREATE TABLE [dbo].[Gold]					\
				(											\
					[id] INT NOT NULL PRIMARY KEY IDENTITY,	\
					[gold] INT NULL							\
				)";

		FDataBaseConnection* Connection = GDataBaseConnectionPool->Get();
		check(Connection->Execute(Query));
		GDataBaseConnectionPool->Release(Connection);
	}

	for (int32 i = 0; i < 3; ++i)
	{
		FDataBaseConnection* Connection = GDataBaseConnectionPool->Get();

		// 기존에 바인딩된 정보 먼저 날림
		Connection->Unbind();

		// 넘길 인자 바인딩
		int32 Gold = 100;
		SQLLEN Len = 0;
		check(Connection->BindParam(
			1, 
			SQL_C_LONG, 
			SQL_INTEGER, 
			0, 
			&Gold, 
			&Len)
		);

		// SQL 실행
		check(Connection->Execute(L"INSERT INTO [dbo].[Gold]([gold]) VALUES(?)"));
		GDataBaseConnectionPool->Release(Connection);
	}

	{
		FDataBaseConnection* Connection = GDataBaseConnectionPool->Get();

		// 기존에 바인딩된 정보 먼저 날림
		Connection->Unbind();

		// 넘길 인자 바인딩
		int32 Gold = 100;
		SQLLEN Len = 0;
		check(Connection->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(Gold), &Gold, &Len));

		int32 OutId = 0;
		SQLLEN OutIdLen = 0;
		check(Connection->BindCol(1, SQL_C_LONG, &OutId, sizeof(OutId), &OutIdLen));

		int32 OutGold = 0;
		SQLLEN OutGoldLen = 0;
		check(Connection->BindCol(2, SQL_C_LONG, &OutGold, sizeof(OutGold), &OutGoldLen));

		check(Connection->Execute(L"SELECT id, gold FROM [dbo].[Gold] WHERE gold = (?)"));

		while (Connection->Fetch())
		{
			// 3개의 데이터가 있으므로
			printf("[%d: %d]\n", OutId, OutGold);
		}

		GDataBaseConnectionPool->Release(Connection);
	}

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
