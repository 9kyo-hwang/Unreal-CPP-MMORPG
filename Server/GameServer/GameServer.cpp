#include "pch.h"
#include <WinSock2.h>	// for Socket Programming
#include <WS2tcpip.h>

#include "MemoryBase.h"
#include "ThreadManager.h"
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* Ftn)
{
	printf("Error at %s(): %ld\n", Ftn, ::WSAGetLastError());
}

struct FSession
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

enum class EIoTypes
{
	Read,
	Write,
	Accept,
	Connect,
};

struct OverlappedEx : WSAOVERLAPPED
{
	OverlappedEx(EIoTypes InType)
		: Type(InType)
	{
		ZeroMemory(this, sizeof(OverlappedEx));
	}

	EIoTypes Type;
};

void WorkerThreadMain(HANDLE CompletionPort)
{
	while (true)
	{
		DWORD BytesTransferred = 0;
		FSession* Session = nullptr;
		OverlappedEx* Overlapped = nullptr;

		// Thread 대기
		bool Result = ::GetQueuedCompletionStatus(
			CompletionPort,
			&BytesTransferred,
			reinterpret_cast<PULONG_PTR>(&Session),
			reinterpret_cast<LPOVERLAPPED*>(&Overlapped),
			INFINITE
		);

		// 네트워크 이벤트 완료 시 CP를 관찰하고 있는 쓰레드 하나가 호출됨
		if (Result == false || BytesTransferred == 0)
		{
			// TODO: 연결 끊김
			continue;
		}

		check(Overlapped->Type == EIoTypes::Read);
		cout << "Recv Data IOCP = " << BytesTransferred << endl;

		// 만약 다시 Recv를 하고 싶다면, 새로운 WSABUF를 만들어 WSARecv를 다시 호출해줘야 함
		// 일종의 낚시마냥, 입질이 오면 건진 뒤 다시 낚싯대를 던져야 함

		WSABUF Buffer(FSession::BufferSize, Session->Buffer);
		DWORD BytesRecv = 0;
		DWORD Flags = 0;
		::WSARecv(Session->Socket, &Buffer, 1, &BytesRecv, &Flags, Overlapped, nullptr);
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
		::WSACleanup();
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

	// 최초 CP 생성 시 or 소켓을 CP에 등록할 때 사용
	HANDLE CompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	vector<FSession*> SessionManager;

	// Worker Thread 생성: CP 관찰, 완료된 IO 작업을 받아서 처리
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->AddThread([=]()
			{
				WorkerThreadMain(CompletionPort);
			});
	}

	// Accept은 Main Thread에서 처리
	while (true)
	{
		SOCKADDR_IN ClientAddr;
		int32 AddrLen = sizeof(ClientAddr);

		// 원활한 테스트를 위해 동기 방식 사용
		SOCKET ClientSocket = ::accept(ListenSocket, reinterpret_cast<SOCKADDR*>(&ClientAddr), &AddrLen);
		if (ClientSocket == INVALID_SOCKET)
		{
			break;
		}

		cout << "Client Connected!" << endl;

		// 여러 쓰레드가 들어오는 상황에 대비해, 힙 영역에 생성 및 일종의 세션 매니저가 모든 세션 관리
		FSession* Session = New<FSession>(ClientSocket);
		SessionManager.push_back(Session);

		// 만들어진 CP에 소켓 등록
		::CreateIoCompletionPort(
			reinterpret_cast<HANDLE>(ClientSocket),
			CompletionPort,
			/*세션 구분을 위한 키(아무거나 가능)*/reinterpret_cast<ULONG_PTR>(Session),
			/*동시 사용 가능한 스레드 개수. 0을 넣으면 최대 개수로 세팅*/0
		);

		WSABUF Buffer(FSession::BufferSize, Session->Buffer);
		DWORD BytesRecv = 0;
		DWORD Flags = 0;
		OverlappedEx* Overlapped = new OverlappedEx(EIoTypes::Read);

		::WSARecv(ClientSocket, &Buffer, 1, &BytesRecv, &Flags, Overlapped, nullptr);
		// Recv나 Send, AcceptEx 등 여러 함수를 호출할 수 있음 -> Overlapped에 Type 정보를 들고 있는 이유

		FSession* OutSession = SessionManager.back();
		SessionManager.pop_back();
		Delete(OutSession);
	}

	GThreadManager->WaitForCompletion();

	::WSACleanup();

	return 0;
}
