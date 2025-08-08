#pragma once
#include <sql.h>
#include <sqlext.h>

class FDataBaseConnection
{
public:
	FDataBaseConnection();
	~FDataBaseConnection();

	bool Open(SQLHENV Environment, const TCHAR* ConnectionString);
	void Clear();

	bool Execute(const TCHAR* CommandString);
	bool Fetch();
	bool GetRowCount(int32& OutRowCount);
	void Unbind();

	// State를 통해 Query 인자를 넘겨줌
	bool BindParam(SQLUSMALLINT ParameterNumber, SQLSMALLINT ValueType, SQLSMALLINT ParameterType, SQLULEN ColumnSize, SQLPOINTER ParameterValuePtr, SQLLEN* StrLen_or_IndPtr);
	// 데이터를 받아올 때 호출하는 메서드
	bool BindCol(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLULEN BufferLength, SQLLEN* StrLen_or_IndPtr);
	void HandleError(SQLRETURN RetVal);

private:
	SQLHDBC Connection;
	SQLHSTMT Statement;
};

