#pragma once

#define USE_LOCKS(count)		FCriticalSection LockObjects[count];
#define USE_LOCK				USE_LOCKS(1)
#define WRITE_LOCK_INDEX(index) FScopeLock ScopeLock_##index(LockObjects[index]);
#define WRITE_LOCK				WRITE_LOCK_INDEX(0)

#ifdef _DEBUG

#else
#endif

#define CRASH(cause)						\
{											\
	uint32* Crash = nullptr;				\
	__analysis_assume(Crash != nullptr);	\
	*Crash = 0xDEADBEEF;					\
}

#define check(Expression)				\
{										\
	if(!(Expression))					\
	{									\
		CRASH("assert");				\
		__analysis_assume(Expression);	\
	}									\
}

//#define _STOMP	// 주석 처리를 해서 Pool 방식으로 스위치 가능