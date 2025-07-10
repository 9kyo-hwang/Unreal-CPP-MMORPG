#include "pch.h"
#include <WinSock2.h>	// for Socket Programming
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")	// 반드시 필요

void HandleError(SOCKET SocketToClose, const char* Ftn)
{
	printf("Error at %s(): %ld\n", Ftn, ::WSAGetLastError());
	//::closesocket(SocketToClose);
	//::WSACleanup();
}

int main()
{
	// Initialize Network
	WSADATA Data;
	int32 Result = ::WSAStartup(MAKEWORD(2, 2), /*거의 사용할 일 없음*/&Data);
	if (Result != NO_ERROR)
	{
		printf("WSAStartup failed: %d\n", Result);
		return 1;
	}

	SOCKET ClientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);	// 일종의 번호(Descriptor)
	if (ClientSocket == INVALID_SOCKET)
	{
		HandleError(ClientSocket, "socket");
		return 1;
	}

	SOCKADDR_IN ServerAddr
	{
		.sin_family = AF_INET,
		.sin_port = ::htons(7777),
		.sin_addr = {},
		.sin_zero = {},
	};
	::inet_pton(AF_INET, "127.0.0.1", &ServerAddr.sin_addr);

	::connect(ClientSocket, reinterpret_cast<SOCKADDR*>(&ServerAddr), sizeof(ServerAddr));

	while (true)
	{
		for (int32 i = 0; i < 10; ++i)
		{
			char SendBuf[100] = "Hello, Server!";
			if (::send(ClientSocket, SendBuf, sizeof(SendBuf), 0) == SOCKET_ERROR)
			{
				HandleError(ClientSocket, "send");
				break;
			}

			printf("Sent data: %lld\t[%s]\n", sizeof(SendBuf), SendBuf);
		}

		SOCKADDR_IN SenderAddr{};	// 반드시 서버라는 보장은 없음.
		int32 SenderAddrSize = sizeof(SenderAddr);

		char RecvBuf[1000];
		Result = ::recv(ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
		if (Result < 0)
		{
			HandleError(ClientSocket, "recv");
		}
		else if (Result == 0)
		{
			printf("Connection Closed\n");
		}
		else
		{
			printf("Echoed: %d\t[%s]\n", Result, RecvBuf);
		}

		this_thread::sleep_for(1s);
	}

	::closesocket(ClientSocket);
	::WSACleanup();

	return 0;
}
