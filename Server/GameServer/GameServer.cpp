#include "pch.h"
#include <WinSock2.h>	// for Socket Programming
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* Ftn)
{
	printf("Error at %s(): %ld\n", Ftn, ::WSAGetLastError());
}

struct FSession : WSAOVERLAPPED
{
	static constexpr int32 BufferSize = 1024;

	SOCKET Socket;
	char Buffer[BufferSize];
	int32 BytesRecv;

	explicit FSession(SOCKET InSocket = INVALID_SOCKET)
		: Socket(InSocket)
		, Buffer{}
		, BytesRecv(0)
	{
		
	}

	~FSession()
	{
		
	}

	void Clear()
	{
		BytesRecv = 0;
	}
};

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
		::WSACleanup();
		return 1;
	}

	u_long ArgP = 1;
	if (::ioctlsocket(ListenSocket, FIONBIO, &ArgP) == INVALID_SOCKET)
	{
		return 1;
	}

	SOCKADDR_IN ServerAddress(AF_INET, ::htons(7777));
	ServerAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);

	if (::bind(ListenSocket, reinterpret_cast<SOCKADDR*>(&ServerAddress), sizeof(ServerAddress)) == SOCKET_ERROR
		|| ::listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		::closesocket(ListenSocket);
		::WSACleanup();
		return 1;
	}

	cout << "Server 대기 중..." << endl;

	auto ReceiveCallback = [](DWORD Error, DWORD Transferred, LPWSAOVERLAPPED OverlappedPtr, DWORD Flags)
		{
			if (Error != 0)
			{
				HandleError("ReceiveCallback");
				return;
			}

			FSession* Session = reinterpret_cast<FSession*>(OverlappedPtr);

			// Echo 서버를 만들고자 하면 여기서 WSASend를 다시 호출해주면 됨
			cout << "Data Recv Len Callback = " << Transferred << endl;
		};

	while (true)
	{
		SOCKADDR_IN ClientAddr;
		int32 AddrLen = sizeof(ClientAddr);

		SOCKET ClientSocket;
		while (true)
		{
			ClientSocket = ::accept(ListenSocket, reinterpret_cast<SOCKADDR*>(&ClientAddr), &AddrLen);
			if (ClientSocket != INVALID_SOCKET)
			{
				break;
			}

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}

			// 에러가 있는 상황
			return 1;
		}

		cout << "Client Connected!" << endl;
		FSession Session(ClientSocket);

		while (true)
		{
			// Buffer 데이터 조작을 절대 해서는 안됨
			WSABUF Buffer(FSession::BufferSize, Session.Buffer);
			DWORD BytesRecv = 0;
			DWORD Flags = 0;
			if (::WSARecv(ClientSocket, &Buffer, 1, &BytesRecv, &Flags, &Session, ReceiveCallback) == SOCKET_ERROR)
		 	{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Pending
					// Alertable Wait
					::SleepEx(INFINITE, true);
					//::WSAWaitForMultipleEvents(1, &Session.Event, true, WSA_INFINITE, true);
				}
				else
				{
					// TODO: 문제 있는 상황
					break;
				}
			}
			else
			{
				cout << "Data Recv Len = " << BytesRecv << endl;
			}
		}

		::closesocket(Session.Socket);
	}

	::WSACleanup();

	return 0;
}
