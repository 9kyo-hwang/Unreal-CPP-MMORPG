#include "pch.h"

#include "Sockets.h"
#include "SocketSubsystem.h"

int main()
{
	FSocket* Socket = FSocketSubsystem::CreateSocket();
	Socket->Bind(7777);
	Socket->Listen();
	SOCKET Client = ::accept(Socket->GetNativeSocket(), nullptr, nullptr);
	cout << "Client Connected!" << endl;

	while (true)
	{
		
	}

	return 0;
}
