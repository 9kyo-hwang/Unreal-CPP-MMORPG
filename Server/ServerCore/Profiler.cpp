#include "pch.h"
#include "Profiler.h"

void FDeadLockProfiler::Add(const char* Name)
{
	FScopeLock Lock(Mutex);

	// ID 찾기 or 발급
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

	// TODO: 스레드마다 잡고 있는 Lock이 다르기 때문에, LockStack 또한 스레드마다 들고 있어야 함
	if (!LockStack.empty())
	{
		// 새로운 Lock이라면 데드락 여부 재확인
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
		if (OrderList[To] == -1)	// 방문한 적 없으면 재귀 호출
		{
			Parent[To] = From;
			DFS(To);
			continue;
		}

		if (OrderList[From] < OrderList[To])	// From을 To보다 먼저 방문했다면 순방향
		{
			continue;
		}

		if (!Visited[To])	// 순방향이 아닌데 아직 방문하지 않은 다음 노드가 있다면 역방향
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
