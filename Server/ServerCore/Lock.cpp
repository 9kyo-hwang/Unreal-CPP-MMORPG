#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "Profiler.h"

void FRWLock::ReadLock(const char* Name)
{
#if _DEBUG
	GDeadLockProfiler->Add(Name);
#endif

	// 동일한 스레드가 Write Lock을 소유하고 있다면 Read Lock은 즉시 성공
	const uint32 LockThreadID = (Flag.load() & static_cast<uint32>(ERWLockFlags::WriteThread)) >> 16;
	if (LThreadID == LockThreadID)
	{
		Flag.fetch_add(1);	// Read Count + 1
		return;
	}

	// 아무도 Write Lock을 소유하고 있지 않다면, 경합해서 카운트(플래그의 하위 16비트) 증가
	const int64 BeginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 NumSpin = 0; NumSpin < NumMaxSpin; ++NumSpin)
		{
			// ReadCount Flag과 &를 하면 Write 부분은 0으로 밀림. 즉 Write Lock을 아무도 안 잡고 있는 것과 같은 의미
			uint32 Expected = Flag.load() & static_cast<uint32>(ERWLockFlags::ReadCount);
			if (Flag.compare_exchange_strong(Expected, Expected + 1))
			{
				// Write Lock이 없으면서, Read Count를 1 증가시키는 것에 성공했음
				return;
			}
		}

		if (::GetTickCount64() - BeginTick >= TimeoutTick)
		{
			// 대부분 DeadLock과 같은 컨텐츠 로직 꼬임
			CRASH("LOCK TIMEOUT");
		}

		// Write Lock을 누군가 잡고 있었거나, 다른 스레드가 나보다 먼저 Read Lock을 잡아버린 경우 실패
		this_thread::yield();
	}
}

void FRWLock::ReadUnlock(const char* Name)
{
#if _DEBUG
	GDeadLockProfiler->Remove(Name);
#endif

	// Read Count를 1 감소시키기 전부터 0이었다면 문제
	if ((Flag.fetch_sub(1) & static_cast<uint32>(ERWLockFlags::ReadCount)) == 0)
	{
		CRASH("Duplicated Unlock");
	}
}

void FRWLock::WriteLock(const char* Name)
{
#if _DEBUG
	GDeadLockProfiler->Add(Name);
#endif

	// 동일한 스레드가 이미 Lock을 소유하고 있다면 즉시 성공
	const uint32 LockThreadID = (Flag.load() & static_cast<uint32>(ERWLockFlags::WriteThread)) >> 16;
	if (LThreadID == LockThreadID)
	{
		++NumWrite;
		return;
	}

	// Write Lock을 아무도 소유하지 않고 있다면, 경합을 통해 소유권 획득
	const uint32 Desired = LThreadID << 16 & static_cast<uint32>(ERWLockFlags::WriteThread);
	const int64 BeginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 NumSpin = 0; NumSpin < NumMaxSpin; ++NumSpin)
		{
			uint32 OutExpected = static_cast<uint32>(ERWLockFlags::Empty);
			if (Flag.compare_exchange_strong(OutExpected, Desired))
			{
				++NumWrite;
				return;
			}
		}

		if (::GetTickCount64() - BeginTick >= TimeoutTick)
		{
			// 대부분 DeadLock과 같은 컨텐츠 로직 꼬임
			CRASH("LOCK TIMEOUT");
		}

		this_thread::yield();
	}
}

void FRWLock::WriteUnlock(const char* Name)
{
#if _DEBUG
	GDeadLockProfiler->Remove(Name);
#endif

	if ((Flag.load() & static_cast<uint32>(ERWLockFlags::ReadCount)) != 0)
	{
		CRASH("INVALID Unlock Order");
	}

	const int32 ExpectedNumWrite = --NumWrite;
	if (ExpectedNumWrite == 0)
	{
		Flag.store(static_cast<uint32>(ERWLockFlags::Empty));
	}
}
