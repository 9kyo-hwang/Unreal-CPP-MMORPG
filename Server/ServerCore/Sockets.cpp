#include "pch.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "SocketSubsystem.h"

bool FSocket::Close()
{
	if (Socket != INVALID_SOCKET)
	{
		int32 Error = ::closesocket(Socket);
		Socket = INVALID_SOCKET; // 소켓을 닫은 후에는 INVALID_SOCKET로 초기화
		return Error == 0;
	}

	return false;
}

bool FSocket::Bind(const FInternetAddr& Addr)
{
	return ::bind(Socket, reinterpret_cast<const SOCKADDR*>(Addr.GetRawAddr()), sizeof(SOCKADDR_IN)) == 0;
}

// TEMP. How to use Bind Any Address using FInternetAddr::SetAnyAddress?
bool FSocket::Bind(uint16 Port)
{
	SOCKADDR_IN Addr(AF_INET, ::htons(Port));
	Addr.sin_addr.S_un.S_addr = INADDR_ANY; // 모든 인터페이스에 바인딩
	return ::bind(Socket, reinterpret_cast<SOCKADDR*>(&Addr), sizeof(Addr)) == 0;
}

bool FSocket::Listen(int32 MaxBacklog)
{
	return ::listen(Socket, MaxBacklog) == 0;
}

bool FSocket::SetNoDelay(bool bIsNoDelay)
{
	return SetSockOpt(Socket, IPPROTO_TCP, TCP_NODELAY, bIsNoDelay) == 0;
}

bool FSocket::SetReuseAddr(bool bAllowReuse)
{
	return SetSockOpt(Socket, SOL_SOCKET, SO_REUSEADDR, bAllowReuse) == 0;
}

bool FSocket::SetLinger(bool bShouldLinger, int32 Timeout)
{
	LINGER Linger(bShouldLinger, static_cast<u_short>(Timeout));
	return SetSockOpt(Socket, SOL_SOCKET, SO_LINGER, Linger) == 0;
}

bool FSocket::SetSendBufferSize(int32 Size, int32& NewSize)
{
	socklen_t SizeSize = sizeof(int32);
	bool bOk = SetSockOpt(Socket, SOL_SOCKET, SO_SNDBUF, Size) == 0;

	// Read the value back in case the size was modified
	::getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&NewSize), &SizeSize);
	return bOk;
}

bool FSocket::SetReceiveBufferSize(int32 Size, int32& NewSize)
{
	socklen_t SizeSize = sizeof(int32);
	bool bOk = SetSockOpt(Socket, SOL_SOCKET, SO_RCVBUF, Size) == 0;

	// Read the value back in case the size was modified
	::getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&NewSize), &SizeSize);
	return bOk;
}

bool FSocket::SetUpdateAcceptSocket(SOCKET ListenSocket)
{
	return SetSockOpt(Socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, ListenSocket) == 0;
}

