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
	void OnReceive(BYTE* buffer, int32 len) override;
	void OnSend(int32 len) override;

public:
	TWeakPtr<class Room> _room;
};