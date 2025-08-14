#include "pch.h"
#include "DataBaseConnection.h"

FDatabaseConnection::FDatabaseConnection()
	: Connection(nullptr)
	, Statement(nullptr)
{
}

FDatabaseConnection::~FDatabaseConnection()
{
}

bool FDatabaseConnection::Open(SQLHENV Environment, const TCHAR* ConnectionString)
{
	if (::SQLAllocHandle(
		SQL_HANDLE_DBC, 
		Environment,
		&Connection) != SQL_SUCCESS
		)
	{
		return false;
	}

	TCHAR ConnStrIn[MAX_PATH]{};
	::wcscpy_s(ConnStrIn, ConnectionString);

	TCHAR ConnStrOut[MAX_PATH]{};
	SQLSMALLINT OutLength = 0;

	SQLRETURN bResult = ::SQLDriverConnectW(
		Connection, 
		nullptr, 
		ConnStrIn, 
		countof(ConnStrIn), 
		ConnStrOut, 
		countof(ConnStrOut), 
		&OutLength, 
		SQL_DRIVER_NOPROMPT
	);

	if (::SQLAllocHandle(SQL_HANDLE_STMT, Connection, &Statement) != SQL_SUCCESS)
	{
		return false;
	}

	return bResult == SQL_SUCCESS || bResult == SQL_SUCCESS_WITH_INFO;
}

void FDatabaseConnection::Clear()
{
	if (Connection != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, Connection);
		Connection = nullptr;
	}

	if (Statement != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, Statement);
		Statement = nullptr;
	}
}

bool FDatabaseConnection::Execute(const TCHAR* CommandString)
{
	SQLRETURN bResult = ::SQLExecDirectW(Statement, const_cast<SQLWCHAR*>(CommandString), SQL_NTSL);
	if (bResult == SQL_SUCCESS || bResult == SQL_SUCCESS_WITH_INFO)
	{
		return true;
	}

	HandleError(bResult);
	return false;
}

bool FDatabaseConnection::Fetch()
{
	switch (SQLRETURN bResult = ::SQLFetch(Statement))
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		return true;

	case SQL_NO_DATA:
		return false;

	case SQL_ERROR:
		HandleError(bResult);
		return false;

	default:
		return true;
	}
}

bool FDatabaseConnection::GetRowCount(int32& OutRowCount)
{
	SQLLEN RowCount = 0;
	SQLRETURN bResult = ::SQLRowCount(Statement, &RowCount);

	if (bResult == SQL_SUCCESS || bResult == SQL_SUCCESS_WITH_INFO)
	{
		OutRowCount = static_cast<int32>(RowCount);
		return true;
	}

	return false;
}

void FDatabaseConnection::Unbind()
{
	// BindParams를 통해 인자를 넘기고, BindCol을 통해 받아오기도 함
	// 따라서 DB를 사용하기 전 이전의 흔적을 지울 필요가 있음
	::SQLFreeStmt(Statement, SQL_UNBIND);
	::SQLFreeStmt(Statement, SQL_RESET_PARAMS);
	::SQLFreeStmt(Statement, SQL_CLOSE);
}

bool FDatabaseConnection::BindParam(int32 ParameterNumber, bool* ParameterValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindParam(ParameterNumber, SQL_C_TINYINT, SQL_TINYINT, sizeof(bool), ParameterValuePtr, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindParam(int32 ParameterNumber, float* ParameterValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindParam(ParameterNumber, SQL_C_FLOAT, SQL_FLOAT, 0, ParameterValuePtr, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindParam(int32 ParameterNumber, double* ParameterValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindParam(ParameterNumber, SQL_C_DOUBLE, SQL_DOUBLE, 0, ParameterValuePtr, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindParam(int32 ParameterNumber, int8* ParameterValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindParam(ParameterNumber, SQL_C_TINYINT, SQL_TINYINT, sizeof(int8), ParameterValuePtr, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindParam(int32 ParameterNumber, int16* ParameterValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindParam(ParameterNumber, SQL_C_SHORT, SQL_SMALLINT, sizeof(int16), ParameterValuePtr, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindParam(int32 ParameterNumber, int32* ParameterValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindParam(ParameterNumber, SQL_C_LONG, SQL_INTEGER, sizeof(int32), ParameterValuePtr, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindParam(int32 ParameterNumber, int64* ParameterValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindParam(ParameterNumber, SQL_C_SBIGINT, SQL_BIGINT, sizeof(int64), ParameterValuePtr, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindParam(int32 ParameterNumber, TIMESTAMP_STRUCT* ParameterValuePtr,
	SQLLEN* StrLen_or_IndPtr)
{
	return BindParam(ParameterNumber, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), ParameterValuePtr, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindParam(int32 ParameterNumber, const TCHAR* ParameterValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	SQLULEN Size = ::wcslen(ParameterValuePtr + 1) * 2;	// null char
	*StrLen_or_IndPtr = SQL_NTSL;

	return Size > WVARCHAR_MAX
		? BindParam(ParameterNumber, SQL_C_WCHAR, SQL_WLONGVARCHAR, Size, const_cast<TCHAR*>(ParameterValuePtr), StrLen_or_IndPtr)
		: BindParam(ParameterNumber, SQL_C_WCHAR, SQL_WVARCHAR, Size, const_cast<TCHAR*>(ParameterValuePtr), StrLen_or_IndPtr);
}

// 이미지 파일 같이 클라이언트에서 사용하는 Binary 파일
bool FDatabaseConnection::BindParam(int32 ParameterNumber, const BYTE* ParameterValuePtr, int32 Size, SQLLEN* StrLen_or_IndPtr)
{
	if (ParameterValuePtr == nullptr)
	{
		*StrLen_or_IndPtr = SQL_NULL_DATA;
		Size = 1;
	}
	else
	{
		*StrLen_or_IndPtr = Size;
	}

	return Size > BINARY_MAX
		? BindParam(ParameterNumber, SQL_C_BINARY, SQL_LONGVARBINARY, Size, const_cast<BYTE*>(ParameterValuePtr), StrLen_or_IndPtr)
		: BindParam(ParameterNumber, SQL_C_BINARY, SQL_BINARY, Size, const_cast<BYTE*>(ParameterValuePtr), StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, bool* TargetValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_TINYINT, TargetValuePtr, sizeof(bool), StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, float* TargetValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_FLOAT, TargetValuePtr, sizeof(float), StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, double* TargetValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_DOUBLE, TargetValuePtr, sizeof(double), StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, int8* TargetValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_TINYINT, TargetValuePtr, sizeof(int8), StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, int16* TargetValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_SHORT, TargetValuePtr, sizeof(int16), StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, int32* TargetValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_LONG, TargetValuePtr, sizeof(int32), StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, int64* TargetValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_SBIGINT, TargetValuePtr, sizeof(int64), StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, TIMESTAMP_STRUCT* TargetValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_TYPE_TIMESTAMP, TargetValuePtr, sizeof(TIMESTAMP_STRUCT), StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, TCHAR* TargetValuePtr, int32 Size, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_WCHAR, TargetValuePtr, Size, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindCol(int32 ColumnNumber, BYTE* TargetValuePtr, int32 Size, SQLLEN* StrLen_or_IndPtr)
{
	return BindCol(ColumnNumber, SQL_C_BINARY, TargetValuePtr, Size, StrLen_or_IndPtr);
}

bool FDatabaseConnection::BindParam(SQLUSMALLINT ParameterNumber, SQLSMALLINT ValueType, SQLSMALLINT ParameterType, SQLULEN ColumnSize,
                                    SQLPOINTER ParameterValuePtr, SQLLEN* StrLen_or_IndPtr)
{
	SQLRETURN RetVal = ::SQLBindParameter(
		Statement, 
		ParameterNumber, 
		SQL_PARAM_INPUT, 
		ValueType, 
		ParameterType, 
		ColumnSize, 
		0, 
		ParameterValuePtr, 
		0, 
		StrLen_or_IndPtr
	);

	if (RetVal != SQL_SUCCESS && RetVal != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(RetVal);
		return false;
	}

	return true;
}

bool FDatabaseConnection::BindCol(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLULEN BufferLength,
	SQLLEN* StrLen_or_IndPtr)
{
	SQLRETURN RetVal = ::SQLBindCol(
		Statement, 
		ColumnNumber,
		TargetType,
		TargetValuePtr, 
		BufferLength,
		StrLen_or_IndPtr
	);

	if (RetVal != SQL_SUCCESS && RetVal != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(RetVal);
		return false;
	}

	return true;
}

void FDatabaseConnection::HandleError(SQLRETURN RetVal)
{
	if (RetVal == SQL_SUCCESS)
	{
		return;
	}

	SQLSMALLINT Index = 1;
	SQLWCHAR SqlState[MAX_PATH]{};
	SQLINTEGER NativeError = 0;
	SQLWCHAR ErrorMsg[MAX_PATH]{};
	SQLSMALLINT MsgLen = 0;

	while (true)
	{
		SQLRETURN bResult = ::SQLGetDiagRecW(
			SQL_HANDLE_STMT,
			Statement,
			Index,
			SqlState,
			&NativeError,
			ErrorMsg,
			countof(ErrorMsg),
			&MsgLen
		);

		if (bResult == SQL_NO_DATA || bResult == SQL_SUCCESS || bResult == SQL_SUCCESS_WITH_INFO)
		{
			break;
		}

		wcout.imbue(locale("kor"));
		wcout << ErrorMsg << endl;
		++Index;
	}
}
