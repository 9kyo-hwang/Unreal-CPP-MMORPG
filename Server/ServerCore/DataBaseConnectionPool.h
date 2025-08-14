#pragma once
#include "DataBaseConnection.h"

class FDatabaseConnectionPool
{
public:
	FDatabaseConnectionPool();
	~FDatabaseConnectionPool();

	bool Open(int32 NumConnection, const TCHAR* ConnectionString);
	void Clear();

	// 사용하자마자 바로 Pool에 반납할 것이라 스마트포인터 사용 X
	FDatabaseConnection* Get();
	void Release(FDatabaseConnection* Connection);

private:
	USE_LOCK;
	SQLHENV Environment;
	TArray<FDatabaseConnection*> Connections;
};

