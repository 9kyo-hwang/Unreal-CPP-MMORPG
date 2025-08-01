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
		// [ServerPacket_Test]
		ServerPacketWriter_Test PacketWriter(1001, 100, 10);

		// [ServerPacket_Test][FBuffData FBuffData FBuffData]
		ServerPacketWriter_Test::FBuffDataArray BuffDataArray = PacketWriter.ReserveBuffs(3);
		BuffDataArray[0] = { 100, 1.5f };
		BuffDataArray[1] = { 200, 2.3f };
		BuffDataArray[2] = { 300, 0.7f };

		// [ServerPacket_Test][FBuffData FBuffData FBuffData][Victim Victim Victim][Victim][Victim Victim]
		auto VictimArray1 = PacketWriter.ReserveVictims(&BuffDataArray[0], 3);
		{
			VictimArray1[0] = 1001; // Victim Ids
			VictimArray1[1] = 1002;
			VictimArray1[2] = 1003;
		}

		auto VictimArray2 = PacketWriter.ReserveVictims(&BuffDataArray[1], 1);
		{
			VictimArray2[0] = 2001; // Victim Ids
		}

		auto VictimArray3 = PacketWriter.ReserveVictims(&BuffDataArray[2], 2);
		{
			VictimArray3[0] = 3001; // Victim Ids
			VictimArray3[1] = 3002;
		}

		shared_ptr<FSendBuffer> SendBuffer = PacketWriter.Close();
		GSessionManager.Broadcast(SendBuffer); // Broadcast to all sessions

		this_thread::sleep_for(250ms);
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
