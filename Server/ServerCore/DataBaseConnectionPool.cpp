#include "pch.h"
#include "DataBaseConnectionPool.h"

FDatabaseConnectionPool::FDatabaseConnectionPool()
	: Environment(nullptr)
{
}

FDatabaseConnectionPool::~FDatabaseConnectionPool()
{
	Clear();
}

bool FDatabaseConnectionPool::Open(int32 NumConnection, const TCHAR* ConnectionString)
{
	// 최초 서버를 실행할 때 1회 수행
	WRITE_LOCK;

	// 흐름만 확인
	if (::SQLAllocHandle(
		SQL_HANDLE_ENV, 
		SQL_NULL_HANDLE, 
		&Environment) != SQL_SUCCESS
		)
	{
		return false;
	}

	if (::SQLSetEnvAttr(
		Environment, 
		SQL_ATTR_ODBC_VERSION, 
		reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 
		0) != SQL_SUCCESS
		)
	{
		return false;
	}

	for (int32 i = 0; i < NumConnection; ++i)
	{
		// 커넥션 수 만큼 스레드 생성
		FDatabaseConnection* Connection = New<FDatabaseConnection>();
		if (!Connection->Open(Environment, ConnectionString))
		{
			return false;
		}

		Connections.push_back(Connection);
	}

	return true;
}

void FDatabaseConnectionPool::Clear()
{
	WRITE_LOCK;
	if (Environment != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, Environment);
		Environment = SQL_NULL_HANDLE;
	}

	for (FDatabaseConnection* Connection : Connections)
	{
		Delete(Connection);
	}

	Connections.clear();
}

FDatabaseConnection* FDatabaseConnectionPool::Get()
{
	WRITE_LOCK;
	if (Connections.empty())
	{
		return nullptr;
	}

	FDatabaseConnection* Connection = Connections.back();
	Connections.pop_back();
	return Connection;
}

void FDatabaseConnectionPool::Release(FDatabaseConnection* Connection)
{
	WRITE_LOCK;
	Connections.push_back(Connection);
}
