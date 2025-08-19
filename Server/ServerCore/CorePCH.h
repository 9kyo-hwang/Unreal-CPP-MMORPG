#pragma once

#include "Types.h"
#include "CoreMacro.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <iostream>
#include <chrono>
#include <windows.h>
using namespace std;

#include <WinSock2.h>	// for Socket Programming
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "ws2_32.lib")

#include "SendBuffer.h"
#include "Session.h"
#include "AsyncTaskQueue.h"