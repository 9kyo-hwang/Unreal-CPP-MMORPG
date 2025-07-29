#include "pch.h"
#include <Service.h>
#include <Session.h>

#include "BufferReader.h"
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
		// 여기에 진입했다는 것은 온전한 패킷이 보장됨
		FBufferReader br(Buffer, Length);
		FPacketHeader PacketHeader; br >> PacketHeader;

		// Server Main에서 보낸 패킷 데이터 순서대로 꺼내야 함
		uint64 Id;
		uint32 Hp;
		uint16 Atk;
		br >> Id >> Hp >> Atk;

		printf("ID: %llu, HP: %u, ATK: %u\n", Id, Hp, Atk);

		char RecvBuffer[4096];
		// 헤더에 명시된 [헤더 + 데이터] 크기 - [헤더] 크기 - [id] 크기 - [hp] 크기 - [atk] 크기
		// 추후 가변 길이 데이터에 대한 크기 정보도 보내게 될 것
		br.Read(RecvBuffer, PacketHeader.Size - sizeof(FPacketHeader) - 8 - 4 - 2);
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
