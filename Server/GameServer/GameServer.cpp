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

	int32 SendBufferSize;
	int32 SndbufOptLen = sizeof(SendBufferSize);
	::getsockopt(ServerSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&SendBufferSize), &SndbufOptLen);
	cout << "Send Buffer Size: " << SendBufferSize << endl;

	int32 RecvBufferSize;
	int32 RcvbufOptLen = sizeof(RecvBufferSize);
	::getsockopt(ServerSocket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&RecvBufferSize), &RcvbufOptLen);
	cout << "Receive Buffer Size: " << RecvBufferSize << endl;

	::WSACleanup();

	return 0;
}
