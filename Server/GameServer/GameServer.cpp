#include "pch.h"

#include "BufferWriter.h"
#include "GameSession.h"
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
		Protocol::S_TEST Packet;	// 세션이 해당 정보를 들고 있게 해도 됨
		Packet.set_id(1001);
		Packet.set_hp(100);
		Packet.set_attack(10);

		{
			Protocol::BuffData* Buff = Packet.add_buffs();
			Buff->set_buffid(100);
			Buff->set_remaintime(1.2f);
			Buff->add_victims(4000);
		}
		{
			Protocol::BuffData* Buff = Packet.add_buffs();
			Buff->set_buffid(200);
			Buff->set_remaintime(2.5f);
			Buff->add_victims(1000);
			Buff->add_victims(2000);
		}

		auto SendBuffer = ClientPacketHandler::CreateSendBuffer(Packet);
		GSessionManager.Broadcast(SendBuffer); // Broadcast to all sessions

		this_thread::sleep_for(250ms);
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
