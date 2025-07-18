#pragma once

class FInternetAddr
{
public:
	void SetIp(const SOCKADDR_IN& IpAddr);
	void SetIp(const TCHAR* InAddr, uint16 Port);

	void GetIp(FString& OutAddr) const;
	const SOCKADDR_IN* GetRawAddr() const
	{
		return &Addr;
	}

	int32 GetPort() const;
	void SetAnyAddress();

private:
	SOCKADDR_IN Addr = {};
};

