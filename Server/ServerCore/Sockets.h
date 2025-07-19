#pragma once

using SocketType = SOCKET;
static constexpr SocketType InvalidSocket = ~static_cast<SocketType>(0);

class FInternetAddr;

class FSocket
{
public:
	FSocket(SocketType InSocket);
	~FSocket();

public:
	SocketType GetNativeSocket() const
	{
		return Socket;
	}

public:
	bool Close();
	bool Bind(const FInternetAddr& Addr);
	bool Listen(int32 MaxBacklog = SOMAXCONN);

	bool SetReuseAddr(bool bAllowReuse = true);
	bool SetNoDelay(bool bIsNoDelay = true);
	bool SetLinger(bool bShouldLinger, int32 Timeout);
	bool SetSendBufferSize(int32 Size, int32& NewSize);
	bool SetReceiveBufferSize(int32 Size, int32& NewSize);
	bool SetUpdateAcceptSocket(const unique_ptr<FSocket>& ListenSocket);

private:
	SocketType Socket;
};

