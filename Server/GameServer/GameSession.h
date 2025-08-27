#pragma once
#include "Session.h"

class APlayer;

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

	void ChangePlayer(TSharedPtr<APlayer> NewPlayer) { Player.store(NewPlayer); }

private:
	TAtomic<TSharedPtr<APlayer>> Player;	// Session에서는 Player가 바뀔 수도 있음
};