#pragma once
#include "Session.h"

class FGameSession : public FPacketSession
{
public:
	~FGameSession() override
	{
		cout << "~FGameSession" << endl;
	}

	void OnConnected() override;
	void OnDisconnected() override;
	void OnReceive(BYTE* Buffer, int32 Length) override;
	void OnSend(int32 Length) override;
};