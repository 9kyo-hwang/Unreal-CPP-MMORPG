#pragma once
#include "NetAddress.h"

/*-------------
	FSocket
--------------*/
class FSocket
{
public:
	FSocket();
	FSocket(SOCKET InSocket);
	~FSocket();

	bool SetLinger(uint16 OnOff, uint16 Linger);
	bool SetReuseAddress(bool bFlag);
	bool SetRecvBufferSize(int32 Size);
	bool SetSendBufferSize(int32 Size);
	bool SetTcpNoDelay(bool bFlag);
	bool SetUpdateAcceptSocket(SOCKET ListenSocket);

	bool Bind(FNetAddress InNetAddr) const;
	bool Listen(int32 Backlog = SOMAXCONN) const;
	void Close();

	SOCKET GetSocket() const { return Socket; }

private:
	template<typename T>
	bool SetSockOpt(int32 InLevel, int32 InOptName, T InOptVal)
	{
		return SOCKET_ERROR != ::setsockopt(Socket, InLevel, InOptName, reinterpret_cast<char*>(&InOptVal), sizeof(T));
	}

private:
	SOCKET Socket = INVALID_SOCKET;
};
