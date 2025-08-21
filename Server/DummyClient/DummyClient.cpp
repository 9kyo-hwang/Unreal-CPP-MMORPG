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

	virtual void OnConnected() override
	{
		cout << "OnConnected" << endl;
		
		Protocol::C_ENTER_GAME pkt;
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
	}

	virtual void OnReceive(BYTE* buffer, int32 len) override
	{
		FPacketSessionRef session = GetPacketSessionRef();
		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);

		// TODO : packetId 대역 체크
		ClientPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}
};

int main()
{
	ClientPacketHandler::Init();

	this_thread::sleep_for(1s);

	FClientServiceRef service = make_shared<FClientService>(
		NetAddress(L"127.0.0.1", 7777),
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