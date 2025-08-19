#pragma once
#include "Session.h"

// 상대방을 대표하는 세션
class FClientSession : public FPacketSession
{
public:
	FClientSession();
	~FClientSession() override;

	void OnConnected() override;
	void OnDisconnected() override;
	void OnReceive(BYTE* Buffer, int32 Length) override;
	void OnSend(int32 BytesSent) override;

	weak_ptr<class AGameModeBase> BelongTo;
};