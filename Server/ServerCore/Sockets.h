#pragma once

class FInternetAddr;

class FSocket
{
public:
	FSocket(SOCKET InSocket)
		: Socket(InSocket)
	{
	}

	SOCKET GetNativeSocket()
	{
		return Socket;
	}

public:
	bool Close();
	bool Bind(const FInternetAddr& Addr);
	bool Bind(uint16 Port);
	bool Listen(int32 MaxBacklog = SOMAXCONN);

	bool SetReuseAddr(bool bAllowReuse = true);
	bool SetNoDelay(bool bIsNoDelay = true);
	bool SetLinger(bool bShouldLinger, int32 Timeout);
	bool SetSendBufferSize(int32 Size, int32& NewSize);
	bool SetReceiveBufferSize(int32 Size, int32& NewSize);
	bool SetUpdateAcceptSocket(SOCKET ListenSocket);

private:
	SOCKET Socket;
};

