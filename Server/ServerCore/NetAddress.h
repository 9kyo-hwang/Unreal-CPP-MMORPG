#pragma once

/*--------------
	FNetAddress
---------------*/

class FNetAddress
{
public:
	FNetAddress() = default;
	FNetAddress(SOCKADDR_IN InSockAddr);
	FNetAddress(wstring Ip, uint16 Port);

	const SOCKADDR_IN& GetSockAddr() { return SockAddr; }
	wstring	GetIpAddress() const;
	uint16 GetPort() const { return ::ntohs(SockAddr.sin_port); }

public:
	static IN_ADDR	Ip2Address(const WCHAR* Ip);

private:
	SOCKADDR_IN		SockAddr{};
};

