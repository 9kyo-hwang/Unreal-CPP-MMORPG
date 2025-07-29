#pragma once
#include "Session.h"

// 상대방을 대표하는 세션
class FGameSession : public FPacketSession
{
public:
	void OnConnected() override;
	void OnDisconnected() override;
	int32 OnReceive(BYTE* Buffer, int32 Length) override;
	void OnSend(int32 BytesSent) override;
};