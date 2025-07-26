#pragma once

#include "Types.h"
#include "CoreMacro.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"
#include "Container.h"

#include <iostream>
#include <chrono>
#include <windows.h>
using namespace std;

#include <WinSock2.h>	// for Socket Programming
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "ws2_32.lib")

#include "Lock.h"
#include "ObjectPool.h"
#include "Casts.h"
#include "MemoryBase.h"
#include "SendBuffer.h"