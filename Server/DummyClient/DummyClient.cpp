#include "pch.h"

#include <WinSock2.h>	// for Socket Programming
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")	// 반드시 필요

int main()
{
	uint16 VersionRequested = MAKEWORD(2, 2);
	WSADATA OutData;
	if (::WSAStartup(VersionRequested, /*거의 사용할 일 없음*/&OutData) != 0)
	{
		cout << "[WSAStartup] function failed with error: " << ::WSAGetLastError() << endl;
		return 1;
	}

	uint16 AddressFamily = AF_INET;	// IPv4
	int32 Type = SOCK_STREAM;	// AF_INET + TCP
	int32 Protocol = 0;	// 내부적으로 알아서 프로토콜을 세팅해줌. 우리의 경우 TCP가 선택됨
	uint64 Client = ::socket(AddressFamily, Type, Protocol);	// 일종의 번호(Descriptor)
	if (Client == INVALID_SOCKET)
	{
		cout << "[socket] function failed with error: " << ::WSAGetLastError() << endl;
		return 1;
	}

	uint16 HostPort = 7777;
	SOCKADDR_IN ServerAddress
	{
		.sin_family = AddressFamily,
		.sin_port = ::htons(HostPort),
		.sin_addr = {},
		.sin_zero = {},
	};

	const char* AddressString = "127.0.0.1";	// 일단 내 컴퓨터
	IN_ADDR* AddressBuffer = &ServerAddress.sin_addr;
	if (::inet_pton(AddressFamily, AddressString, AddressBuffer) != 1)
	{
		cout << "[inet_pton] function failed with error: " << ::WSAGetLastError() << endl;
		return 1;
	}

	if (::connect(Client, reinterpret_cast<SOCKADDR*>(&ServerAddress), sizeof(ServerAddress)) == SOCKET_ERROR)
	{
		cout << "[connect] function failed with error: " << ::WSAGetLastError() << endl;
		return 1;
	}

	cout << "Connected To Server!" << endl;

	while (true)
	{
		// TODO

		this_thread::sleep_for(1s);
	}

	::closesocket(Client);
	::WSACleanup();

	return 0;
}
