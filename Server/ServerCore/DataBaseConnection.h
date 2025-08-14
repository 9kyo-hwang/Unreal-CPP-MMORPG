#pragma once
#include <sql.h>
#include <sqlext.h>

static constexpr uint64 BINARY_MAX = 8000;	// page 단위가 8kb -> max 또한 8000
static constexpr uint64 WVARCHAR_MAX = 4000;	// using wchar_t -> final length is double of string size

class FDatabaseConnection
{
public:
	FDatabaseConnection();
	~FDatabaseConnection();

	bool Open(SQLHENV Environment, const TCHAR* ConnectionString);
	void Clear();

	bool Execute(const TCHAR* CommandString);
	bool Fetch();
	bool GetRowCount(int32& OutRowCount);
	void Unbind();

public:
	bool BindParam(int32 ParameterNumber, bool*				ParameterValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindParam(int32 ParameterNumber, float*			ParameterValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindParam(int32 ParameterNumber, double*			ParameterValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindParam(int32 ParameterNumber, int8*				ParameterValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindParam(int32 ParameterNumber, int16*			ParameterValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindParam(int32 ParameterNumber, int32*			ParameterValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindParam(int32 ParameterNumber, int64*			ParameterValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindParam(int32 ParameterNumber, TIMESTAMP_STRUCT*	ParameterValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindParam(int32 ParameterNumber, const TCHAR*		ParameterValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindParam(int32 ParameterNumber, const BYTE*		ParameterValuePtr, int32 Size, SQLLEN* StrLen_or_IndPtr);

	bool BindCol(int32 ColumnNumber, bool*				TargetValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindCol(int32 ColumnNumber, float*				TargetValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindCol(int32 ColumnNumber, double*			TargetValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindCol(int32 ColumnNumber, int8*				TargetValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindCol(int32 ColumnNumber, int16*				TargetValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindCol(int32 ColumnNumber, int32*				TargetValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindCol(int32 ColumnNumber, int64*				TargetValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindCol(int32 ColumnNumber, TIMESTAMP_STRUCT*	TargetValuePtr,             SQLLEN* StrLen_or_IndPtr);
	bool BindCol(int32 ColumnNumber, TCHAR*				TargetValuePtr, int32 Size, SQLLEN* StrLen_or_IndPtr);
	bool BindCol(int32 ColumnNumber, BYTE*				TargetValuePtr, int32 Size, SQLLEN* StrLen_or_IndPtr);

private:
	// Statement를 통해 Query 인자를 넘겨줌
	bool BindParam(SQLUSMALLINT ParameterNumber, SQLSMALLINT ValueType, SQLSMALLINT ParameterType, SQLULEN ColumnSize, SQLPOINTER ParameterValuePtr, SQLLEN* StrLen_or_IndPtr);
	// 데이터를 받아올 때 호출하는 메서드
	bool BindCol(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLULEN BufferLength, SQLLEN* StrLen_or_IndPtr);
	void HandleError(SQLRETURN RetVal);

private:
	SQLHDBC Connection;
	SQLHSTMT Statement;
};