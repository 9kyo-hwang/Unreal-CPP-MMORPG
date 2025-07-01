#pragma once

/**
 *	RW SpinLock(MMO에서는 Event 방식보다 적합하다고 판단)
 *	하위 16bit는 Write Flag:	Exclusive Lock Owner Thread ID
 *	상위 16bit는 Read Flag:	Shared Lock Count
 *
 *	Read Lock이 잡힌 상태에서는 Write Lock을 해제할 수 없음에 주의
 */
class FRWLock
{
public:
	void ReadLock(const char* Name);
	void ReadUnlock(const char* Name);
	void WriteLock(const char* Name);
	void WriteUnlock(const char* Name);

public:
	const uint32 TimeoutTick = 10000;
	const uint32 NumMaxSpin = 5000;

	enum class ERWLockFlags : uint32
	{
		Empty = 0x0000'0000,
		ReadCount = 0x0000'FFFF,
		WriteThread = 0xFFFF'0000,
	};

private:
	TAtomic<uint32> Flag = static_cast<uint32>(ERWLockFlags::Empty);
	uint32 NumWrite = 0;	// 재귀적 락 호출을 허용하기 위해. Lock을 잡은 스레드만 사용하므로 Atomic은 필요 없음
};

enum FRWScopeLockType
{
	SLT_ReadOnly = 0,
	SLT_Write,
};

class FRWScopeLock
{
public:
	FRWScopeLock(FRWLock& InLockObject, FRWScopeLockType InLockType, const char* InName)
		: LockObject(InLockObject)
		, LockType(InLockType)
		, Name(InName)
	{
		switch (InLockType)
		{
		case SLT_ReadOnly:
			LockObject.ReadLock(Name);
			break;
		case SLT_Write:
			LockObject.WriteLock(Name);
			break;
		}
	}

	~FRWScopeLock()
	{
		switch (LockType)
		{
		case SLT_ReadOnly:
			LockObject.ReadUnlock(Name);
			break;
		case SLT_Write:
			LockObject.WriteUnlock(Name);
			break;
		}
	}

private:
	FRWLock& LockObject;
	FRWScopeLockType LockType;
	const char* Name;
};