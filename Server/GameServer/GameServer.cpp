#include "pch.h"
#include <WinSock2.h>	// for Socket Programming
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

struct FSession
{
	static constexpr int32 BufferSize = 1024;

	SOCKET Socket;
	char Buffer[BufferSize];
	int32 BytesSent;
	int32 BytesRecv;

	explicit FSession(SOCKET InSocket = INVALID_SOCKET)
		: Socket(InSocket)
		, Buffer{}
		, BytesSent(0)
		, BytesRecv(0)
	{
	}

	void Clear()
	{
		BytesRecv = BytesSent = 0;
	}
};

struct FSocketSet : fd_set
{
	FSocketSet()
	{
		Initialize();
	}

	void Initialize()
	{
		FD_ZERO(this);
	}

	void Clear(SOCKET Socket)
	{
		FD_CLR(Socket, this);
	}

	bool Contains(SOCKET Socket)
	{
		return FD_ISSET(Socket, this);
	}

	// Accept할 소켓을 추가합니다.
	void Add(SOCKET Socket)
	{
		FD_SET(Socket, this);
	}
};

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

	vector<FSession> Sessions;
	Sessions.reserve(100);

	FSocketSet RecvSet, SendSet;

	while (true)
	{
		RecvSet.Initialize();
		SendSet.Initialize();

		// Listen Socket 등록
		RecvSet.Add(ListenSocket);

		for (FSession& Session : Sessions)
		{
			// Echo 서버를 구현하기 때문에, 수신/송신 바이트 수를 이용해 구분
			if (Session.BytesRecv <= Session.BytesSent)
			{
				RecvSet.Add(Session.Socket);
			}
			else
			{
				SendSet.Add(Session.Socket);
			}
		}

		int32 NumReadySocketHandles = ::select(/*Windows에서는 사용하지 않음*/0, &RecvSet, &SendSet, nullptr, /*대기할 시간*/nullptr);
		if (NumReadySocketHandles == SOCKET_ERROR)
		{
			break;
		}
		else if (NumReadySocketHandles == 0)
		{
			// time limit expired
			// 우리는 timeout을 설정하지 않아 진입하지 않음
			break;
		}

		if (RecvSet.Contains(ListenSocket))
		{
			// 소켓이 Set에 존재한다는 건 select 함수로부터 제거되지 않았다는 뜻 -> 클라이언트가 접속 요청을 한 것!
			SOCKADDR_IN ClientAddr;
			int32 AddrLen = sizeof(ClientAddr);

			SOCKET ClientSocket = ::accept(ListenSocket, reinterpret_cast<SOCKADDR*>(&ClientAddr), &AddrLen);
			// 원래는 INVALID_SOCKET를 체크해야 했으나, ReadSet을 이용해 들어온 것을 이미 확인
			if (ClientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected!" << endl;
				Sessions.emplace_back(ClientSocket);
			}
		}

		// 나머지 소켓에 대해서도 Set에 포함됐는지 포함
		for (FSession& Session : Sessions)
		{
			if (RecvSet.Contains(Session.Socket))
			{
				int32 BytesRecv = ::recv(Session.Socket, Session.Buffer, FSession::BufferSize, 0);
				if (BytesRecv <= 0)
				{
					// TODO: 해당 세션을 Set에서 제거
					continue;
				}

				Session.BytesRecv = BytesRecv;
			}

			// OS의 송신 버퍼에 빈 공간이 있어 복사할 수 있음
			if (SendSet.Contains(Session.Socket))
			{
				// 논블로킹 소켓은 상대방 수신 버퍼 상황에 따라 데이터를 일부만 보낼 수 있음(어지간하면 전체를 다 보내도록 설계돼있음)
				// 따라서 남은 데이터만 전송하도록 설계(어지간하면 0부터 시작해서 전체 데이터)
				int32 BytesSent = ::send(Session.Socket, &Session.Buffer[Session.BytesSent], Session.BytesRecv - Session.BytesSent, 0);
				if (BytesSent == SOCKET_ERROR)
				{
					// TODO: 해당 세션을 Set에서 제거
					continue;
				}

				Session.BytesSent += BytesSent;
				if (Session.BytesRecv == Session.BytesSent)
				{
					// Echo 서버이므로 수신 크기 == 송신 크기 -> 데이터를 온전히 다 주고받음
					// 따라서 송수신 데이터 크기를 0으로 초기화
					Session.Clear();
				}
			}
		}
	}

	::WSACleanup();

	return 0;
}
