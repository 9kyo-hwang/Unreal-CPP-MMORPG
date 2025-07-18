#include "pch.h"
#include "SocketSubsystem.h"

#include "Sockets.h"

void FSocketSubsystem::Init()
{
	WSADATA WSAData;
	check(::WSAStartup(MAKEWORD(2, 2), &WSAData) == 0);

	// 런타임에 주소를 얻어오는 API
	FSocket* DummySocket = CreateSocket();
	SOCKET Socket = DummySocket->GetNativeSocket();
	check(Bind(Socket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&Connect)));
	check(Bind(Socket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&Disconnect)));
	check(Bind(Socket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&Accept)));
}

void FSocketSubsystem::Shutdown()
{
	::WSACleanup();
}

bool FSocketSubsystem::Bind(SOCKET Socket, GUID FunctionID, LPVOID* FunctionPointer)
{
	// 런타임에 ConnectEx, DisconnectEx, AcceptEx를 가져와서 사용하기 위해
	DWORD Bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(
		Socket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&FunctionID, 
		sizeof(FunctionID), 
		FunctionPointer, 
		sizeof(FunctionPointer), 
		&Bytes, 
		nullptr, 
		nullptr
	);
}

FSocket* FSocketSubsystem::CreateSocket()
{
	SOCKET Socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	return new FSocket(Socket);
}
