#pragma once

#define USE_LOCKS(count)		FRWLock LockObjects[count];
#define USE_LOCK				USE_LOCKS(1)
#define READ_LOCK_INDEX(index)	FRWScopeLock ReadLock_##index(LockObjects[index], FRWScopeLockType::SLT_ReadOnly, typeid(this).name());
#define READ_LOCK				READ_LOCK_INDEX(0)
#define WRITE_LOCK_INDEX(index) FRWScopeLock WriteLock_##index(LockObjects[index], FRWScopeLockType::SLT_Write, typeid(this).name());
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