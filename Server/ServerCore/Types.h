#pragma once

#include <mutex>
#include <memory>

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

/*----------------------
	 Pointer Cast
----------------------*/

template<typename CastToType, typename CastFromType>
[[nodiscard]] __forceinline std::shared_ptr<CastToType> StaticCastSharedPtr(const std::shared_ptr<CastFromType>& Ptr)
{
	return std::static_pointer_cast<CastToType>(Ptr);
}

template<typename CastToType, typename CastFromType>
[[nodiscard]] __forceinline std::shared_ptr<CastToType> DynamicCastSharedPtr(const std::shared_ptr<CastFromType>& Ptr)
{
	return std::dynamic_pointer_cast<CastToType>(Ptr);
}

template<typename CastToType, typename CastFromType>
[[nodiscard]] __forceinline std::shared_ptr<CastToType> ConstCastSharedPtr(const std::shared_ptr<CastFromType>& Ptr)
{
	return std::const_pointer_cast<CastToType>(Ptr);
}

#define USING_SHARED_PTR(name)	using name##Ref = std::shared_ptr<class name>;

USING_SHARED_PTR(FSocketIOEventQueue);
USING_SHARED_PTR(ISocketIOEventable);
USING_SHARED_PTR(FSession);
USING_SHARED_PTR(FPacketSession);
USING_SHARED_PTR(FListener);
USING_SHARED_PTR(FServerService);
USING_SHARED_PTR(FClientService);
USING_SHARED_PTR(FSendBuffer);
USING_SHARED_PTR(FTask);
USING_SHARED_PTR(FTaskQueue);

#define Sizeof(arr)		static_cast<int32>(sizeof(arr)/sizeof(arr[0]))

//#define _STOMP