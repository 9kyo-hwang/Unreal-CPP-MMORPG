#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "Profiler.h"

void FRWLock::ReadLock(const char* Name)
{
#if _DEBUG
	GDeadLockProfiler->Add(Name);
#endif

	// ������ �����尡 Write Lock�� �����ϰ� �ִٸ� Read Lock�� ��� ����
	const uint32 LockThreadID = (Flag.load() & static_cast<uint32>(ERWLockFlags::WriteThread)) >> 16;
	if (LThreadID == LockThreadID)
	{
		Flag.fetch_add(1);	// Read Count + 1
		return;
	}

	// �ƹ��� Write Lock�� �����ϰ� ���� �ʴٸ�, �����ؼ� ī��Ʈ(�÷����� ���� 16��Ʈ) ����
	const int64 BeginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 NumSpin = 0; NumSpin < NumMaxSpin; ++NumSpin)
		{
			// ReadCount Flag�� &�� �ϸ� Write �κ��� 0���� �и�. �� Write Lock�� �ƹ��� �� ��� �ִ� �Ͱ� ���� �ǹ�
			uint32 Expected = Flag.load() & static_cast<uint32>(ERWLockFlags::ReadCount);
			if (Flag.compare_exchange_strong(Expected, Expected + 1))
			{
				// Write Lock�� �����鼭, Read Count�� 1 ������Ű�� �Ϳ� ��������
				return;
			}
		}

		if (::GetTickCount64() - BeginTick >= TimeoutTick)
		{
			// ��κ� DeadLock�� ���� ������ ���� ����
			CRASH("LOCK TIMEOUT");
		}

		// Write Lock�� ������ ��� �־��ų�, �ٸ� �����尡 ������ ���� Read Lock�� ��ƹ��� ��� ����
		this_thread::yield();
	}
}

void FRWLock::ReadUnlock(const char* Name)
{
#if _DEBUG
	GDeadLockProfiler->Remove(Name);
#endif

	// Read Count�� 1 ���ҽ�Ű�� ������ 0�̾��ٸ� ����
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

	// ������ �����尡 �̹� Lock�� �����ϰ� �ִٸ� ��� ����
	const uint32 LockThreadID = (Flag.load() & static_cast<uint32>(ERWLockFlags::WriteThread)) >> 16;
	if (LThreadID == LockThreadID)
	{
		++NumWrite;
		return;
	}

	// Write Lock�� �ƹ��� �������� �ʰ� �ִٸ�, ������ ���� ������ ȹ��
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
			// ��κ� DeadLock�� ���� ������ ���� ����
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
