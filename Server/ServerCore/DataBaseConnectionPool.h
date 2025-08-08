#pragma once
#include "DataBaseConnection.h"

class FDataBaseConnectionPool
{
public:
	FDataBaseConnectionPool();
	~FDataBaseConnectionPool();

	bool Open(int32 NumConnection, const TCHAR* ConnectionString);
	void Clear();

	// 사용하자마자 바로 Pool에 반납할 것이라 스마트포인터 사용 X
	FDataBaseConnection* Get();
	void Release(FDataBaseConnection* Connection);

private:
	USE_LOCK;
	SQLHENV Environment;
	TArray<FDataBaseConnection*> Connections;
};

