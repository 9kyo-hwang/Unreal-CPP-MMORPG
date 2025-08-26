#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "ClientPacketHandler.h"

char sendData[] = "Hello World";

class FServerSession : public FPacketSession
{
public:
	~FServerSession() override
	{
		cout << "~FServerSession" << endl;
	}

	void OnConnected() override
	{
		cout << "OnConnected" << endl;
		
		Protocol::C_ENTER_GAME pkt;
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
	}

	void OnReceive(BYTE* Buffer, int32 Length) override
	{
		FPacketSessionRef Session = GetPacketSessionRef();
		PacketHeader* Header = reinterpret_cast<PacketHeader*>(Buffer);

		// TODO : packetId 대역 체크
		ClientPacketHandler::HandlePacket(Session, Buffer, Length);
	}

	void OnSend(int32 Length) override
	{
		cout << "OnSend Len = " << Length << endl;
	}

	void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}
};

int main()
{
	ClientPacketHandler::Init();

	this_thread::sleep_for(1s);

	FClientServiceRef service = make_shared<FClientService>(
		FNetAddress(L"127.0.0.1", 7777),
		make_shared<FSocketIOEventQueue>(),
		[=]() { return make_shared<FServerSession>(); }, // TODO : SessionManager 등
		1);

	check(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetEventQueue()->Dispatch();
				}
			});
	}

	while (true)
	{
		//service->Broadcast(sendBuffer);
		this_thread::sleep_for(1s);
	}

	GThreadManager->Join();

}