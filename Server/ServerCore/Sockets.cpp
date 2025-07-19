#include "pch.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "SocketSubsystem.h"

FSocket::FSocket(SocketType InSocket)
	: Socket(InSocket)
{
}

FSocket::~FSocket()
{
	Close();
}

bool FSocket::Close()
{
	if (Socket != InvalidSocket)
	{
		int32 Error = ::closesocket(Socket);
		Socket = InvalidSocket; // 소켓을 닫은 후에는 INVALID_SOCKET으로 초기화
		return Error == 0;
	}

	return false;
}

bool FSocket::Bind(const FInternetAddr& Addr)
{
	return ::bind(Socket, reinterpret_cast<const SOCKADDR*>(Addr.GetRawAddr()), sizeof(SOCKADDR_IN)) == 0;
}

bool FSocket::Listen(int32 MaxBacklog)
{
	return ::listen(Socket, MaxBacklog) == 0;
}

bool FSocket::SetNoDelay(bool bIsNoDelay)
{
	return SetSockOpt(Socket, IPPROTO_TCP, TCP_NODELAY, bIsNoDelay);
}

bool FSocket::SetReuseAddr(bool bAllowReuse)
{
	return SetSockOpt(Socket, SOL_SOCKET, SO_REUSEADDR, bAllowReuse);
}

bool FSocket::SetLinger(bool bShouldLinger, int32 Timeout)
{
	LINGER Linger(bShouldLinger, static_cast<u_short>(Timeout));
	return SetSockOpt(Socket, SOL_SOCKET, SO_LINGER, Linger);
}

bool FSocket::SetSendBufferSize(int32 Size, int32& NewSize)
{
	socklen_t SizeSize = sizeof(int32);
	bool bOk = SetSockOpt(Socket, SOL_SOCKET, SO_SNDBUF, Size);

	// Read the value back in case the size was modified
	::getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&NewSize), &SizeSize);
	return bOk;
}

bool FSocket::SetReceiveBufferSize(int32 Size, int32& NewSize)
{
	socklen_t SizeSize = sizeof(int32);
	bool bOk = SetSockOpt(Socket, SOL_SOCKET, SO_RCVBUF, Size);

	// Read the value back in case the size was modified
	::getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&NewSize), &SizeSize);
	return bOk;
}

bool FSocket::SetUpdateAcceptSocket(const unique_ptr<FSocket>& ListenSocket)
{
	return SetSockOpt(Socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, ListenSocket->GetNativeSocket());
}