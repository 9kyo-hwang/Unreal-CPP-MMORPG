#pragma once
#include "NetAddress.h"

/*----------------
	FSocketUtils
-----------------*/

class FSocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;

public:
	static void Init();
	static void Clear();

	static bool BindWindowsFunction(SOCKET Socket, GUID Guid, LPVOID* Function);
	static SOCKET CreateSocket();
};

