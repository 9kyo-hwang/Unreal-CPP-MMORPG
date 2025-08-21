#include "pch.h"
#include "SocketUtils.h"

/*----------------
	FSocketUtils
-----------------*/

LPFN_CONNECTEX		FSocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	FSocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		FSocketUtils::AcceptEx = nullptr;

void FSocketUtils::Init()
{
	WSADATA WSAData;
	check(::WSAStartup(MAKEWORD(2, 2), OUT &WSAData) == 0);

	SOCKET Dummy = CreateSocket();
	check(BindWindowsFunction(Dummy, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	check(BindWindowsFunction(Dummy, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	check(BindWindowsFunction(Dummy, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	::closesocket(Dummy);
}

void FSocketUtils::Clear()
{
	::WSACleanup();
}

bool FSocketUtils::BindWindowsFunction(SOCKET Socket, GUID Guid, LPVOID* Function)
{
	DWORD Bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(
		Socket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&Guid, 
		sizeof(Guid), 
		Function, 
		sizeof(*Function), 
		OUT &Bytes, 
		nullptr, 
		nullptr
	);
}

SOCKET FSocketUtils::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
}