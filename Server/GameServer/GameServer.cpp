#include "pch.h"
#include <WinSock2.h>	// for Socket Programming
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

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

	// UDP에서는 Listen Socket이 필요 없음
	SOCKET ServerSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (ServerSocket == INVALID_SOCKET)
	{
		HandleError(ServerSocket, "socket");
		return 1;
	}

	SOCKADDR_IN ServerAddr
	{
		.sin_family = AF_INET,
		.sin_port = ::htons(7777),
		.sin_addr = {},
		.sin_zero = {},
	};
	ServerAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);

	if (::bind(ServerSocket, 
		reinterpret_cast<SOCKADDR*>(&ServerAddr), 
		sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		HandleError(ServerSocket, "bind");
		return 1;
	}

	do
	{
		SOCKADDR_IN ClientAddr{};
		int32 ClientAddrSize = sizeof(ClientAddr);

		this_thread::sleep_for(1s);

		char RecvBuf[1000];
		Result = ::recvfrom(ServerSocket, RecvBuf, sizeof(RecvBuf), 0, 
			reinterpret_cast<SOCKADDR*>(&ClientAddr), &ClientAddrSize);

		if (Result == SOCKET_ERROR)
		{
			HandleError(ServerSocket, "recvfrom");
		}
		else if (Result == 0)
		{
			printf("Connection Closed\n");
		}
		else
		{
			printf("Received bytes: %d\t[%s]\n", Result, RecvBuf);
			if (::sendto(ServerSocket, RecvBuf, sizeof(RecvBuf), 0, 
				reinterpret_cast<SOCKADDR*>(&ClientAddr), ClientAddrSize) == SOCKET_ERROR)
			{
				HandleError(ServerSocket, "sendto");
			}

			printf("Send bytes: %lld\n", sizeof(RecvBuf));
		}

	} while (Result > 0);

	::WSACleanup();

	return 0;
}
