#include "pch.h"
#include "DataBaseConnection.h"

FDataBaseConnection::FDataBaseConnection()
	: Connection(nullptr)
	, Statement(nullptr)
{
}

FDataBaseConnection::~FDataBaseConnection()
{
}

bool FDataBaseConnection::Open(SQLHENV Environment, const TCHAR* ConnectionString)
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

void FDataBaseConnection::Clear()
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

bool FDataBaseConnection::Execute(const TCHAR* CommandString)
{
	SQLRETURN bResult = ::SQLExecDirectW(Statement, const_cast<SQLWCHAR*>(CommandString), SQL_NTSL);
	if (bResult == SQL_SUCCESS || bResult == SQL_SUCCESS_WITH_INFO)
	{
		return true;
	}

	HandleError(bResult);
	return false;
}

bool FDataBaseConnection::Fetch()
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

bool FDataBaseConnection::GetRowCount(int32& OutRowCount)
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

void FDataBaseConnection::Unbind()
{
	// BindParams를 통해 인자를 넘기고, BindCol을 통해 받아오기도 함
	// 따라서 DB를 사용하기 전 이전의 흔적을 지울 필요가 있음
	::SQLFreeStmt(Statement, SQL_UNBIND);
	::SQLFreeStmt(Statement, SQL_RESET_PARAMS);
	::SQLFreeStmt(Statement, SQL_CLOSE);
}

bool FDataBaseConnection::BindParam(SQLUSMALLINT ParameterNumber, SQLSMALLINT ValueType, SQLSMALLINT ParameterType, SQLULEN ColumnSize,
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

bool FDataBaseConnection::BindCol(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLULEN BufferLength,
	SQLLEN* StrLen_or_IndPtr)
{
	SQLRETURN RetVal = ::SQLBindCol(
		Statement, 
		ColumnNumber,
		TargetType,
		TargetValue, 
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

void FDataBaseConnection::HandleError(SQLRETURN RetVal)
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
