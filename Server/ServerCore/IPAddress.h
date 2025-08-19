#pragma once

class FInternetAddr
{
public:
	FInternetAddr();
	FInternetAddr(const SOCKADDR_IN& InAddr);
	FInternetAddr(const TCHAR* InAddr, uint16 Port);

	void Clear();

public:
	void SetIp(const SOCKADDR_IN& IpAddr);
	void SetIp(const TCHAR* InAddr, uint16 Port);

	void GetIp(wstring& OutAddr) const;
	const SOCKADDR_IN* GetRawAddr() const
	{
		return &Addr;
	}

	int32 GetPort() const;
	void SetPort(uint16 Port);
	void SetAnyAddress();

private:
	SOCKADDR_IN Addr;
};

