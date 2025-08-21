#include "pch.h"
#include "NetAddress.h"

/*--------------
	NetAddress
---------------*/

NetAddress::NetAddress(SOCKADDR_IN InSockAddr) : SockAddr(InSockAddr)
{
}

NetAddress::NetAddress(wstring Ip, uint16 Port)
{
	::memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr = Ip2Address(Ip.c_str());
	SockAddr.sin_port = ::htons(Port);
}

wstring NetAddress::GetIpAddress() const
{
	WCHAR Buffer[100];
	::InetNtopW(AF_INET, &SockAddr.sin_addr, Buffer, len32(Buffer));
	return wstring(Buffer);
}

IN_ADDR NetAddress::Ip2Address(const WCHAR* Ip)
{
	IN_ADDR InAddr;
	::InetPtonW(AF_INET, Ip, &InAddr);
	return InAddr;
}
