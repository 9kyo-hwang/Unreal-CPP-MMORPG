#include "pch.h"
#include <Service.h>
#include <Session.h>

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

	int32 OnReceive(BYTE* Buffer, int32 Length) override
	{
		FPacketHeader PacketHeader = *reinterpret_cast<FPacketHeader*>(Buffer);

		char RecvBuffer[4096];
		::memcpy(RecvBuffer, &Buffer[4], PacketHeader.Size - sizeof(FPacketHeader));
		printf("%s\n", RecvBuffer);

		return Length;
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
		1000
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
