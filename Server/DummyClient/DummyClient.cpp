#include "pch.h"
#include <Service.h>
#include <Session.h>

#include "ServerPacketHandler.h"
#include "ThreadManager.h"

BYTE SendData[] = "Hello, World!";

// 상대방을 대표하는 세션
class FServerSession : public FPacketSession
{
public:
	void OnConnected() override
	{
		/*
		* 1. 연결 성공 시 로그인 요청
		* 원래는 인증 서버가 ID/PW를 처리하는 것도 해야 함
		*/
		Protocol::C_LOGIN Packet;
		auto SendBuffer = ServerPacketHandler::CreateSendBuffer(Packet);
		Send(SendBuffer);
	}

	void OnReceive(BYTE* Buffer, int32 Length) override
	{
		// 여기에 진입했다는 것은 온전한 패킷이 보장됨
		shared_ptr<FPacketSession> Session = SharedThisSession();

		// 추후 서버가 여러 용도로 분산되어 있다면, ID 대역폭을 보고 적절한 핸들러를 적용해야 함
		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);

		// TODO: Packet Id 대역 확인
		ServerPacketHandler::Incoming(Session, Buffer, Length);
	}

	void OnSend(int32 BytesSent) override
	{
		cout << "OnSend Len = " << BytesSent << endl;
	}

	void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}
};

int main()
{
	ServerPacketHandler::Initialize();

	this_thread::sleep_for(1s);

	auto Service = make_shared<FClientService>(
		FInternetAddr(TEXT("127.0.0.1"), 7777),
		make_shared<FSocketEventQueue>(),
		[=]() { return make_shared<FServerSession>(); },	// ()를 붙이면 안됨. 추후 SessionManager 등에서 관리
		1
	);

	check(Service->Run());

	for (int32 i = 0; i < 2; ++i)
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
		// Service->Broadcast(SendBuffer);
		this_thread::sleep_for(1s);
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
