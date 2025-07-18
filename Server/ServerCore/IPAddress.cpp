#include "pch.h"
#include "IPAddress.h"

void FInternetAddr::SetIp(const SOCKADDR_IN& IpAddr)
{
	Addr = IpAddr;
}

void FInternetAddr::SetIp(const TCHAR* InAddr, uint16 Port)
{
	IN_ADDR Address;
	::InetPtonW(AF_INET, InAddr, &Address);

	ZeroMemory(&Addr, sizeof(Addr));
	Addr.sin_addr = Address;
	Addr.sin_family = AF_INET; // IPv4
	Addr.sin_port = Port;
}

void FInternetAddr::GetIp(FString& OutAddr) const
{
	TCHAR Buffer[100];
	::InetNtopW(AF_INET, &Addr.sin_addr, Buffer, countof(Buffer));
	OutAddr = FString(Buffer);
 }

int32 FInternetAddr::GetPort() const
{
	return ::ntohs(Addr.sin_port);
}

void FInternetAddr::SetAnyAddress()
{
	ZeroMemory(&Addr, sizeof(Addr));
	Addr.sin_addr.s_addr = INADDR_ANY;
	Addr.sin_family = AF_INET;
}
