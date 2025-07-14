#include "pch.h"
#include <WinSock2.h>	// for Socket Programming
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* Ftn)
{
	printf("Error at %s(): %ld\n", Ftn, ::WSAGetLastError());
}

int TrySend(const SOCKET& ClientSocket, const char* SendBuffer, int32& OutSendLength)
{
	OutSendLength = ::send(ClientSocket, SendBuffer, sizeof(SendBuffer), 0);

	if (OutSendLength == INVALID_SOCKET)
	{
		return ::WSAGetLastError() == WSAEWOULDBLOCK ? 0 : -1;
	}
	else
	{
		return 1;
	}
}

int TryReceive(const SOCKET& ClientSocket, char* ReceiveBuffer, int32& OutReceiveLength)
{
	OutReceiveLength = ::recv(ClientSocket, ReceiveBuffer, sizeof(ReceiveBuffer), 0);

	if (OutReceiveLength == SOCKET_ERROR)
	{
		return ::WSAGetLastError() == WSAEWOULDBLOCK ? 0 : -1;
	}
	else if (OutReceiveLength == 0)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

int TryAccept(const SOCKET& ListenSocket, SOCKET& OutClientSocket)
{
	SOCKADDR_IN ClientAddress;
	int32 AddressLength = sizeof(ClientAddress);
	OutClientSocket = ::accept(ListenSocket, reinterpret_cast<SOCKADDR*>(&ClientAddress), &AddressLength);

	if (OutClientSocket == INVALID_SOCKET)
	{
		return ::WSAGetLastError() == WSAEWOULDBLOCK ? 0 : -1;
	}
	else
	{
		return 1;
	}
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

	SOCKET ListenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket == INVALID_SOCKET)
	{
		return 1;
	}

	u_long ArgP = 1;
	if (::ioctlsocket(ListenSocket, FIONBIO, &ArgP) == INVALID_SOCKET)
	{
		return 1;
	}

	SOCKADDR_IN ServerAddress(AF_INET, ::htons(7777));
	ServerAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);

	if (::bind(ListenSocket, reinterpret_cast<SOCKADDR*>(&ServerAddress), sizeof(ServerAddress)) == SOCKET_ERROR)
	{
		return 1;
	}

	if (::listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		return 1;
	}

	cout << "Accept!" << endl;

	while (true)
	{
		SOCKET ClientSocket;
		int32 AcceptResult = TryAccept(ListenSocket, ClientSocket);
		if (AcceptResult == -1)
		{
			break;
		}
		else if (AcceptResult == 0)
		{
			continue;
		}

		cout << "Client connected!" << endl;

		while (true)
		{
			char ReceiveBuffer[1000];
			int32 ReceiveLength;
			int32 ReceiveResult = TryReceive(ClientSocket, ReceiveBuffer, ReceiveLength);
			if (ReceiveResult == -1)	// Error or Disconnected
			{
				break;
			}
			else if (ReceiveResult == 0)	// WouldBlock
			{
				continue;
			}

			cout << "Recv Data Len = " << ReceiveLength << endl;

			while (true)
			{
				int32 SendLen;
				int32 SendResult = TrySend(ClientSocket, ReceiveBuffer, SendLen);
				if (SendResult == -1)	// Error
				{
					break;
				}
				else if (SendResult == 1)
				{
					cout << "Send Data! Len = " << SendLen << endl;
					break;
				}
			}
		}
	}

	::WSACleanup();

	return 0;
}
