#pragma once

class FDeadLockProfiler
{
public:
	void Add(const char* Name);
	void Remove(const char* Name);
	void CheckCycle();

private:
	void DFS(int32 From);

private:
	unordered_map<const char*, int32> NameToId;
	unordered_map<int32, const char*> IdToName;
	stack<int32> LockStack;
	map<int32, set<int32>> Graph;

	FCriticalSection Mutex;

private:
	vector<int32> OrderList;	// 노드 방문 순서
	int32 Order = 0;
	vector<bool> Visited;
	vector<int32> Parent;
};
