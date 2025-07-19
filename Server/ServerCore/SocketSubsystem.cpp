#include "pch.h"
#include "SocketSubsystem.h"

#include "Sockets.h"

void FSocketSubsystem::Init()
{
	WSADATA WSAData;
	check(::WSAStartup(MAKEWORD(2, 2), &WSAData) == 0);

	// 런타임에 주소를 가져오는 API
	auto DummySocket = CreateSocket();
	check(LoadSocketFunctions(DummySocket->GetNativeSocket()));
}

void FSocketSubsystem::Shutdown()
{
	::WSACleanup();
}

bool FSocketSubsystem::LoadSocketFunctions(SOCKET Socket)
{
	// 런타임에 ConnectEx, DisconnectEx, AcceptEx의 포인터를 얻어오기 위함
	GUID ConnectExId = WSAID_CONNECTEX;
	GUID DisconnectExId = WSAID_DISCONNECTEX;
	GUID AcceptExId = WSAID_ACCEPTEX;
	DWORD Bytes = 0;

	if (SOCKET_ERROR == ::WSAIoctl(Socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &ConnectExId, sizeof(ConnectExId), &Connect, sizeof(Connect), &Bytes, nullptr, nullptr))
	{
		return false;
	}

	if (SOCKET_ERROR == ::WSAIoctl(Socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &DisconnectExId, sizeof(DisconnectExId), &Disconnect, sizeof(Disconnect), &Bytes, nullptr, nullptr))
	{
		return false;
	}

	if (SOCKET_ERROR == ::WSAIoctl(Socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &AcceptExId, sizeof(AcceptExId), &Accept, sizeof(Accept), &Bytes, nullptr, nullptr))
	{
		return false;
	}

	return true;
}

std::unique_ptr<FSocket> FSocketSubsystem::CreateSocket()
{
	SOCKET Socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (Socket == INVALID_SOCKET)
	{
		return nullptr;
	}

	return std::make_unique<FSocket>(Socket);
}