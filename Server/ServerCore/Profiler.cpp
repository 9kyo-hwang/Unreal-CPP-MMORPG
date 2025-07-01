#include "pch.h"
#include "Profiler.h"

void FDeadLockProfiler::Add(const char* Name)
{
	FScopeLock Lock(Mutex);

	// ID ã�� or �߱�
	int32 Id = 0;
	if (NameToId.contains(Name))
	{
		Id = NameToId.at(Name);
	}
	else
	{
		Id = static_cast<int32>(NameToId.size());
		NameToId.emplace(Name, Id);
		IdToName.emplace(Id, Name);
	}

	// TODO: �����帶�� ��� �ִ� Lock�� �ٸ��� ������, LockStack ���� �����帶�� ��� �־�� ��
	if (!LockStack.empty())
	{
		// ���ο� Lock�̶�� ����� ���� ��Ȯ��
		const int32 PrevId = LockStack.top();
		if (Id != PrevId)
		{
			set<int32>& AdjacentLocks = Graph[PrevId];
			if (!AdjacentLocks.contains(Id))
			{
				AdjacentLocks.emplace(Id);
				CheckCycle();
			}
		}
	}

	LockStack.emplace(Id);
}

void FDeadLockProfiler::Remove(const char* Name)
{
	FScopeLock Lock(Mutex);

	if (LockStack.empty())
	{
		CRASH("Multiple Unlock");
	}

	int32 Id = NameToId.at(Name);
	if (LockStack.top() != Id)
	{
		CRASH("Invalid Unlock");
	}

	LockStack.pop();
}

void FDeadLockProfiler::CheckCycle()
{
	const int32 NumLock = static_cast<int32>(NameToId.size());

	OrderList.assign(NumLock, -1);
	Order = 0;
	Visited.assign(NumLock, false);
	Parent.assign(NumLock, -1);

	for (int32 Id = 0; Id < NumLock; ++Id)
	{
		DFS(Id);
	}

	OrderList.clear();
	Visited.clear();
	Parent.clear();
}

void FDeadLockProfiler::DFS(int32 From)
{
	if (OrderList[From] != -1)
	{
		return;
	}

	OrderList[From] = Order++;
	for (int32 To : Graph[From])
	{
		if (OrderList[To] == -1)	// �湮�� �� ������ ��� ȣ��
		{
			Parent[To] = From;
			DFS(To);
			continue;
		}

		if (OrderList[From] < OrderList[To])	// From�� To���� ���� �湮�ߴٸ� ������
		{
			continue;
		}

		if (!Visited[To])	// �������� �ƴѵ� ���� �湮���� ���� ���� ��尡 �ִٸ� ������
		{
			cout << IdToName[From] << " -> " << IdToName[To] << endl;

			int32 Node = From;
			while (Node != To)
			{
				cout << IdToName[Parent[Node]] << " -> " << IdToName[Node] << endl;
				Node = Parent[Node];
			}

			CRASH("DEADLOCK Detected");
		}
	}

	Visited[From] = true;
}
