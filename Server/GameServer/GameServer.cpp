#include "pch.h"

#include "BufferWriter.h"
#include "GameSession.h"
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

	BYTE SendData[1024] = "Hello, World!";
	while (true)
	{
		shared_ptr<FSendBuffer> SendBuffer = GSendBufferPool->Open(4096);

		FBufferWriter Writer(SendBuffer->GetData(), SendBuffer->GetCapacity());
		FPacketHeader* PacketHeader = Writer.Reserve<FPacketHeader>();

		// id(uint64), hp(uint32), atk(uint16)
		Writer << static_cast<uint64>(1001) << static_cast<uint32>(100) << static_cast<uint16>(10);
		Writer.Write(SendData, sizeof(SendData));

		PacketHeader->Size = Writer.GetWriteSize();
		PacketHeader->Id = 1;	// TODO: Protocol Id

		SendBuffer->Close(Writer.GetWriteSize());

		GSessionManager.Broadcast(SendBuffer); // Broadcast to all sessions

		this_thread::sleep_for(250ms);
	}

	GThreadManager->WaitForCompletion();

	return 0;
}
