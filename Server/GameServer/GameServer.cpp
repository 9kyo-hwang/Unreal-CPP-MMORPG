#include "pch.h"
#include <WinSock2.h>	// for Socket Programming
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

enum class ENetworkEvents : long
{
	Accept = FD_ACCEPT,
	Read = FD_READ,
	Write = FD_WRITE,
	Close = FD_CLOSE,
};

inline ENetworkEvents operator|(ENetworkEvents Lhs, ENetworkEvents Rhs)
{
	return static_cast<ENetworkEvents>(static_cast<long>(Lhs) | static_cast<long>(Rhs));
}

constexpr int32 ErrorBit(ENetworkEvents E)
{
	switch (E)
	{
	case ENetworkEvents::Accept:	return FD_ACCEPT_BIT;
	case ENetworkEvents::Read:		return FD_READ_BIT;
	case ENetworkEvents::Write:		return FD_WRITE_BIT;
	case ENetworkEvents::Close:		return FD_CLOSE_BIT;
	}

	return -1;
}

class WSession
{
public:
	static constexpr int32 BufferSize = 1024;

	explicit WSession(SOCKET InSocket, ENetworkEvents InEventType)
		: Socket(InSocket)
		, Event(::WSACreateEvent())
		, EventType(static_cast<long>(InEventType))
		, NetworkEvents()
		, Buffer{}
		, BytesRecv(0)
		, BytesSent(0)
	{
		u_long Mode = 1;
		::ioctlsocket(Socket, FIONBIO, &Mode);
		::WSAEventSelect(Socket, Event, EventType);
	}

	~WSession()
	{
		if (Socket != INVALID_SOCKET)
		{
			::closesocket(Socket);
		}

		if (Event != WSA_INVALID_EVENT)
		{
			::WSACloseEvent(Event);
		}
	}

	SOCKET GetSocket() const { return Socket; }
	WSAEVENT GetEvent() const { return Event; }

	bool QueryEvents()
	{
		ZeroMemory(&NetworkEvents, sizeof(NetworkEvents));
		return ::WSAEnumNetworkEvents(Socket, Event, &NetworkEvents) != SOCKET_ERROR;
	}

	bool Has(ENetworkEvents InEventType) const
	{
		long Mask = static_cast<long>(InEventType);
		if ((EventType & Mask) == 0)
		{
			return false;
		}

		int32 Bit = ErrorBit(InEventType);
		return NetworkEvents.lNetworkEvents & Mask && NetworkEvents.iErrorCode[Bit] == 0;
	}

	int32 TryReceive()
	{
		if (BytesRecv != 0)
		{
			return 0;
		}

		int32 Bytes = ::recv(Socket, Buffer, BufferSize, 0);
		if (Bytes == SOCKET_ERROR)
		{
			return ::WSAGetLastError() == WSAEWOULDBLOCK ? 0 : -1;
		}

		BytesRecv = Bytes;
		return Bytes;
	}

	int32 TrySend()
	{
		if (BytesRecv <= BytesSent)
		{
			return 0;
		}

		int32 Bytes = ::send(Socket, &Buffer[BytesSent], BytesRecv - BytesSent, 0);
		if (Bytes == SOCKET_ERROR)
		{
			return ::WSAGetLastError() == WSAEWOULDBLOCK ? 0 : -1;
		}

		BytesSent += Bytes;
		if (BytesRecv == BytesSent)	// 다 보냈으면 Clear
		{
			BytesRecv = BytesSent = 0;
		}

		return Bytes;
	}

private:
	SOCKET Socket;
	WSAEVENT Event;
	long EventType;
	WSANETWORKEVENTS NetworkEvents;
	char Buffer[BufferSize];
	int32 BytesRecv;
	int32 BytesSent;
};

void HandleError(const char* Ftn)
{
	printf("Error at %s(): %ld\n", Ftn, ::WSAGetLastError());
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

	vector<unique_ptr<WSession>> Sessions;
	Sessions.emplace_back(
		make_unique<WSession>(
			ListenSocket, 
			ENetworkEvents::Accept | ENetworkEvents::Close
		)
	);

	while (true)
	{
		vector<WSAEVENT> Events;
		for (auto& Session : Sessions)
		{
			Events.emplace_back(Session->GetEvent());
		}

		DWORD Index = ::WSAWaitForMultipleEvents(
			static_cast<DWORD>(Events.size()), 
			Events.data(), 
			false, 
			WSA_INFINITE, 
			false
		);

		if (Index == WSA_WAIT_FAILED)
		{
			continue;
		}

		Index -= WSA_WAIT_EVENT_0;

		const auto Session = Sessions[Index].get();
		if (Session->QueryEvents() == false)
		{
			continue;
		}

		if (Session->Has(ENetworkEvents::Accept))
		{
			SOCKADDR_IN ClientAddr;
			int32 AddrLen = sizeof(ClientAddr);
			SOCKET ClientSocket = ::accept(ListenSocket, reinterpret_cast<SOCKADDR*>(&ClientAddr), &AddrLen);
			if (ClientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected!" << endl;
				Sessions.emplace_back(
					make_unique<WSession>(
						ClientSocket,
						ENetworkEvents::Read | ENetworkEvents::Write | ENetworkEvents::Close
					)
				);
			}
		}

		if (Session->Has(ENetworkEvents::Read) || Session->Has(ENetworkEvents::Write))
		{
			int32 BytesRecv = Session->TryReceive();
			if (BytesRecv > 0)
			{
				cout << "Recv Data = " << BytesRecv << endl;
			}
			else if (BytesRecv < 0)
			{
				// TODO: Erase Session
			}

			int32 BytesSent = Session->TrySend();
			if (BytesSent > 0)
			{
				cout << "Send Data = " << BytesSent << endl;
			}
			else if (BytesSent < 0)
			{
				// TODO: Erase session
			}
		}

		if (Session->Has(ENetworkEvents::Close))
		{
			// TODO: Erase socket
		}
	}

	::WSACleanup();

	return 0;
}
