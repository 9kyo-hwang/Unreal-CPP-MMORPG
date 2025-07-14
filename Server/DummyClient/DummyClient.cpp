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

	::closesocket(ClientSocket);
	::WSACleanup();

	return 0;
}
