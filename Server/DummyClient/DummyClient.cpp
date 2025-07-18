#include "pch.h"
#include <chrono>

void HandleError(const char* Ftn)
{
	printf("Error at %s(): %ld\n", Ftn, ::WSAGetLastError());
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

int main()
{
	// Server보다 먼저 켜지지 않도록
	std::this_thread::sleep_for(1s);

	// Initialize Network
	WSADATA Data;
	int32 Result = ::WSAStartup(MAKEWORD(2, 2), /*거의 사용할 일 없음*/&Data);
	if (Result != NO_ERROR)
	{
		printf("WSAStartup failed: %d\n", Result);
		return 1;
	}

	SOCKET ClientSocket = ::socket(AF_INET, SOCK_STREAM, 0);	// 일종의 번호(Descriptor)
	if (ClientSocket == INVALID_SOCKET)
	{
		return 1;
	}

	u_long ArgP = 1;
	if (::ioctlsocket(ClientSocket, FIONBIO, &ArgP) == INVALID_SOCKET)
	{
		return 1;
	}

	SOCKADDR_IN ServerAddress(AF_INET, ::htons(7777));
	::inet_pton(AF_INET, "127.0.0.1", &ServerAddress.sin_addr);

	while (true)
	{
		if (::connect(ClientSocket, reinterpret_cast<SOCKADDR*>(&ServerAddress), sizeof(ServerAddress)) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// 연결 시도 중
				continue;
			}

			if (::WSAGetLastError() == WSAEISCONN)
			{
				// 앞서 Would Block에 의해 빠꾸먹고 다시 들어온 경우. 이미 연결된 상태라 connect 그만
				break;
			}

			// Error
			break;
		}
	}

	cout << "Connected to server!" << endl;
	char SendBuffer[100] = "Hello, Server!";
	WSAEVENT Event = ::WSACreateEvent();
	WSAOVERLAPPED Overlapped{};
	Overlapped.hEvent = Event;

	while (true)
	{
		WSABUF Buffer(100, SendBuffer);
		DWORD BytesSent = 0;
		DWORD Flags = 0;

		if (::WSASend(ClientSocket, &Buffer, 1, &BytesSent, Flags, &Overlapped, nullptr) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSA_IO_PENDING)
			{
				::WSAWaitForMultipleEvents(1, &Event, true, WSA_INFINITE, false);
				::WSAGetOverlappedResult(ClientSocket, &Overlapped, &BytesSent, false, &Flags);
			}
			else
			{
				// Error occurred
				break;
			}
		}

		cout << "Send Data! Len = " << sizeof(SendBuffer) << endl;

		this_thread::sleep_for(1s);
	}

	::closesocket(ClientSocket);
	::WSACleanup();

	return 0;
}
