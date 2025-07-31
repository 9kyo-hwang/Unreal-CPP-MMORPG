#include "pch.h"
#include <Service.h>
#include <Session.h>

#include "BufferReader.h"
#include "ClientPacketHandler.h"
#include "ThreadManager.h"

BYTE SendData[] = "Hello, World!";

// 상대방을 대표하는 세션
class FServerSession : public FPacketSession
{
public:
	void OnConnected() override
	{
		// cout << "Connected To Server" << endl;
	}

	void OnReceive(BYTE* Buffer, int32 Length) override
	{
		// 여기에 진입했다는 것은 온전한 패킷이 보장됨
		ClientPacketHandler::HandlePacket(Buffer, Length);
	}

	void OnSend(int32 BytesSent) override
	{
		// cout << "OnSend Len = " << BytesSent << endl;
	}

	void OnDisconnected() override
	{
		// cout << "Disconnected" << endl;
	}
};

int main()
{
	this_thread::sleep_for(1s);

	auto Service = MakeShared<FClientService>(
		FInternetAddr(TEXT("127.0.0.1"), 7777),
		MakeShared<FSocketEventQueue>(),
		MakeShared<FServerSession>,	// ()를 붙이면 안됨. 추후 SessionManager 등에서 관리
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

	GThreadManager->WaitForCompletion();

	return 0;
}
