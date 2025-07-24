#include "pch.h"

#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"

// TODO
class FGameSession : public FSession
{
public:
	int32 OnRecv(BYTE* Buffer, int32 Length) override
	{
		cout << "OnRecv Len = " << Length << endl;
		Send(Buffer, Length);	// Echo
		return Length;
	}

	void OnSend(int32 BytesSent) override
	{
		cout << "OnSend Len = " << BytesSent << endl;
	}
};

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

	GThreadManager->WaitForCompletion();

	return 0;
}
