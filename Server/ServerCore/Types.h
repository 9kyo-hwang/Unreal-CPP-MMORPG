#pragma once

#include <mutex>

using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using FCriticalSection = std::mutex;
using FScopeLock = std::lock_guard<std::mutex>;

#define USING_SHARED_PTR(name)	using name##Ref = std::shared_ptr<class name>;

USING_SHARED_PTR(FSocketIOEventQueue);
USING_SHARED_PTR(ISocketIOEventable);
USING_SHARED_PTR(FSession);
USING_SHARED_PTR(FPacketSession);
USING_SHARED_PTR(FListener);
USING_SHARED_PTR(FServerService);
USING_SHARED_PTR(FClientService);
USING_SHARED_PTR(FSendBuffer);
USING_SHARED_PTR(FJob);
USING_SHARED_PTR(FJobQueue);

#define size16(val)		static_cast<int16>(sizeof(val))
#define size32(val)		static_cast<int32>(sizeof(val))
#define len16(arr)		static_cast<int16>(sizeof(arr)/sizeof(arr[0]))
#define len32(arr)		static_cast<int32>(sizeof(arr)/sizeof(arr[0]))

//#define _STOMP