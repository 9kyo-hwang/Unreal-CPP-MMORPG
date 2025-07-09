#include "pch.h"
#include <thread>
#include "CoreGlobal.h"
#include "Allocator.h"
#include "Casts.h"
#include "Memory.h"
#include "MemoryBase.h"
#include "RefCountBase.h"
#include "ThreadManager.h"

#include <WinSock2.h>	// for Socket Programming
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

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
	uint64 Listen = ::socket(AddressFamily, Type, Protocol);	// 일종의 번호(Descriptor)
	if (Listen == INVALID_SOCKET)
	{
		cout << "[socket] function failed with error: " << ::WSAGetLastError() << endl;
		return 1;
	}

	uint16 HostPort = 7777;
	SOCKADDR_IN MyAddress	// 서버 입장에서는 내 주소
	{
		.sin_family = AddressFamily,
		.sin_port = ::htons(HostPort),
		.sin_addr = {},
		.sin_zero = {},
	};

	IN_ADDR& AddressBuffer = MyAddress.sin_addr;
	AddressBuffer.s_addr = ::htonl(INADDR_ANY);	// 고정된 주소가 아닌, 적절한 모든 주소에 연결됨

	if (::bind(Listen, reinterpret_cast<SOCKADDR*>(&MyAddress), sizeof(MyAddress)) == SOCKET_ERROR)
	{
		cout << "[bind] function failed with error: " << ::WSAGetLastError() << endl;
		return 1;
	}

	int32 Backlog = 10;
	if (::listen(Listen, Backlog) == SOCKET_ERROR)
	{
		cout << "[listen] function failed with error: " << ::WSAGetLastError() << endl;
		return 1;
	}

	while (true)
	{
		SOCKADDR_IN ClientAddress{};
		int32 AddressLength = sizeof(ClientAddress);
		uint64 Client = ::accept(Listen, reinterpret_cast<SOCKADDR*>(&ClientAddress), &AddressLength);
		if (Client == INVALID_SOCKET)
		{
			cout << "[accept] function failed with error: " << ::WSAGetLastError() << endl;
			break;
		}

		const uint16 StringBufferSize = INET_ADDRSTRLEN;
		char StringBuffer[StringBufferSize]{};
		if (::inet_ntop(AddressFamily, &ClientAddress.sin_addr, StringBuffer, StringBufferSize) == nullptr)
		{
			cout << "[inet_ntop] function failed with error: " << ::WSAGetLastError() << endl;
			break;
		}

		cout << "Client Connected! IP = "<< StringBuffer << endl;

		// TODO
	}

	::WSACleanup();

	return 0;
}
