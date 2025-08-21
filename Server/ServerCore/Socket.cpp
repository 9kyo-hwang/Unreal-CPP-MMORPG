#include "pch.h"
#include "Socket.h"
#include "SocketUtils.h"

FSocket::FSocket()
{
	Socket = FSocketUtils::CreateSocket();
}

FSocket::FSocket(SOCKET InSocket) : Socket(InSocket)
{
}

FSocket::~FSocket()
{
	Close();
}

bool FSocket::SetLinger(uint16 OnOff, uint16 Linger)
{
	LINGER option;
	option.l_onoff = OnOff;
	option.l_linger = Linger;
	return SetSockOpt(SOL_SOCKET, SO_LINGER, option);
}

bool FSocket::SetReuseAddress(bool bFlag)
{
	return SetSockOpt(SOL_SOCKET, SO_REUSEADDR, bFlag);
}

bool FSocket::SetRecvBufferSize(int32 Size)
{
	return SetSockOpt(SOL_SOCKET, SO_RCVBUF, Size);
}

bool FSocket::SetSendBufferSize(int32 Size)
{
	return SetSockOpt(SOL_SOCKET, SO_SNDBUF, Size);
}

bool FSocket::SetTcpNoDelay(bool bFlag)
{
	return SetSockOpt(SOL_SOCKET, TCP_NODELAY, bFlag);
}

bool FSocket::SetUpdateAcceptSocket(SOCKET ListenSocket)
{
	return SetSockOpt(SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, ListenSocket);
}

bool FSocket::Bind(FNetAddress InNetAddr) const
{
	return SOCKET_ERROR != ::bind(Socket, reinterpret_cast<const SOCKADDR*>(&InNetAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

bool FSocket::Listen(int32 Backlog) const
{
	return SOCKET_ERROR != ::listen(Socket, Backlog);
}

void FSocket::Close()
{
	if (Socket != INVALID_SOCKET)
		::closesocket(Socket);
	Socket = INVALID_SOCKET;
}