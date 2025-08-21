#pragma once

/*--------------
	NetAddress
---------------*/

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN InSockAddr);
	NetAddress(wstring Ip, uint16 Port);

	SOCKADDR_IN&	GetSockAddr() { return SockAddr; }
	wstring			GetIpAddress() const;
	uint16			GetPort() { return ::ntohs(SockAddr.sin_port); }

public:
	static IN_ADDR	Ip2Address(const WCHAR* Ip);

private:
	SOCKADDR_IN		SockAddr{};
};

