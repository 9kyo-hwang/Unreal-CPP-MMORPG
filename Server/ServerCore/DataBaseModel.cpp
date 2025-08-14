#include "pch.h"
#include "DataBaseModel.h"
#include <regex>

FDatabaseColumnInfo::FDatabaseColumnInfo()
	: ColumnId(0)
	, DataType(EDatabaseDataType::None)
	, MaxLen(0)
	, bNullable(false)
	, bIdentity(false)
	, Seed(0)
	, Increment(0)
{
}

FString FDatabaseColumnInfo::CreateText() const
{
	return FDatabaseHelpers::Format(
		TEXT("[%s] %s %s %s"),
		ColumnName.c_str(),
		DataTypeText.c_str(),
		bNullable ? TEXT("NULL") : TEXT("NOT NULL"),
		bIdentity ? FDatabaseHelpers::Format(TEXT("IDENTITY(%d, %d)"), Seed, Increment).c_str() : TEXT("")
	);
}

FDatabaseIndexInfo::FDatabaseIndexInfo()
	: IndexId(0)
	, IndexType(EDatabaseIndexType::NonClustered)
	, bPrimaryKey(false)
	, bUniqueConstraint(false)
{
}

FString FDatabaseIndexInfo::GetUniqueName() const
{
	FString UniqueName;

	UniqueName += bPrimaryKey ? TEXT("PK ") : TEXT(" ");
	UniqueName += bUniqueConstraint ? TEXT("UK ") : TEXT(" ");
	UniqueName += (IndexType == EDatabaseIndexType::Clustered ? TEXT("C ") : TEXT("NC "));

	for (const auto& Column : Columns)
	{
		UniqueName += TEXT("*");
		UniqueName += Column->ColumnName;
		UniqueName += TEXT(" ");
	}

	return UniqueName;
}

FString FDatabaseIndexInfo::CreateName(const FString& TableName) const
{
	FString Name = TEXT("IX_") + TableName;

	for (const auto& Column : Columns)
	{
		Name += L"_";
		Name += Column->ColumnName;
	}

	return Name;
}

FString FDatabaseIndexInfo::GetTypeText() const
{
	return (IndexType == EDatabaseIndexType::Clustered ? TEXT("CLUSTERED") : TEXT("NONCLUSTERED"));
}

FString FDatabaseIndexInfo::GetKeyText() const
{
	if (bPrimaryKey)
	{
		return TEXT("PRIMARY KEY");
	}

	if (bUniqueConstraint)
	{
		return TEXT("UNIQUE");
	}

	return TEXT("");
}

FString FDatabaseIndexInfo::CreateColumnsText() const
{
	FString ColumnsText;

	const int32 size = static_cast<int32>(Columns.size());
	for (int32 i = 0; i < size; i++)
	{
		if (i > 0)
		{
			ColumnsText += L", ";
		}

		ColumnsText += FDatabaseHelpers::Format(L"[%s]", Columns[i]->ColumnName.c_str());
	}

	return ColumnsText;
}

bool FDatabaseIndexInfo::DependsOn(const FString& ColumnName)
{
	return ranges::find_if(Columns,
		[&](const shared_ptr<FDatabaseColumnInfo>& Column)
		{
			return Column->ColumnName == ColumnName;
		}) != Columns.end();
}

FDatabaseTableInfo::FDatabaseTableInfo()
	: ObjectId(0)
{
}

shared_ptr<FDatabaseColumnInfo> FDatabaseTableInfo::GetColumn(const FString& ColumnName)
{
	const auto ColumnPtr = ranges::find_if(Columns,
		[&](const shared_ptr<FDatabaseColumnInfo>& Column)
		{
			return Column->ColumnName == ColumnName;
		});

	if (ColumnPtr != Columns.end())
	{
		return *ColumnPtr;
	}

	return nullptr;
}

FString FDatabaseProcedureInfo::GenerateCreateQuery() const
{
	const TCHAR* Query = TEXT("CREATE PROCEDURE [dbo].[%s] %s AS BEGIN %s END");
	FString Param = GenerateParamString();
	return FDatabaseHelpers::Format(Query, Name.c_str(), Param.c_str(), Body.c_str());
}

FString FDatabaseProcedureInfo::GenerateAlterQuery() const
{
	const TCHAR* Query = TEXT("ALTER PROCEDURE [dbo].[%s] %s AS	BEGIN %s END");
	FString Param = GenerateParamString();
	return FDatabaseHelpers::Format(Query, Name.c_str(), Param.c_str(), Body.c_str());
}

FString FDatabaseProcedureInfo::GenerateParamString() const
{
	FString Param;

	const int32 Size = static_cast<int32>(Params.size());
	for (int32 i = 0; i < Size; i++)
	{
		if (i < Size - 1)
		{
			Param += FDatabaseHelpers::Format(L"\t%s %s,\n", Params[i].Name.c_str(), Params[i].Type.c_str());
		}
		else
		{
			Param += FDatabaseHelpers::Format(L"\t%s %s", Params[i].Name.c_str(), Params[i].Type.c_str());
		}
	}

	return Param;
}

FString FDatabaseHelpers::Format(const TCHAR* Format, ...)
{
	TCHAR Buffer[0x1000];

	va_list Args;
	va_start(Args, Format);
	::vswprintf_s(Buffer, 0x1000, Format, Args);
	va_end(Args);

	return FString(Buffer);
}

FString FDatabaseHelpers::DataType2String(EDatabaseDataType Type)
{
	switch (Type)
	{
	case EDatabaseDataType::TinyInt:	return TEXT("TinyInt");
	case EDatabaseDataType::SmallInt:	return TEXT("SmallInt");
	case EDatabaseDataType::Int:		return TEXT("Int");
	case EDatabaseDataType::Real:		return TEXT("Real");
	case EDatabaseDataType::DateTime:	return TEXT("DateTime");
	case EDatabaseDataType::Float:		return TEXT("Float");
	case EDatabaseDataType::Bit:		return TEXT("Bit");
	case EDatabaseDataType::Numeric:	return TEXT("Numeric");
	case EDatabaseDataType::BigInt:		return TEXT("BigInt");
	case EDatabaseDataType::VarBinary:	return TEXT("VarBinary");
	case EDatabaseDataType::Varchar:	return TEXT("Varchar");
	case EDatabaseDataType::Binary:		return TEXT("Binary");
	case EDatabaseDataType::NVarChar:	return TEXT("NVarChar");
	default:					return TEXT("None");
	}
}

FString FDatabaseHelpers::RemoveWhiteSpace(const FString& Str)
{
	FString Result = Str;
	std::erase_if(Result, [=](WCHAR ch) { return ::isspace(ch); });
	return Result;
}

EDatabaseDataType FDatabaseHelpers::String2DataType(const TCHAR* Str, int32& MaxLen)
{
	std::wregex Regex(L"([a-z]+)(\\((max|\\d+)\\))?");
	std::wcmatch Match;

	if (std::regex_match(Str, Match, Regex) == false)
	{
		return EDatabaseDataType::None;
	}

	MaxLen = Match[3].matched
		? ::_wcsicmp(Match[3].str().c_str(), L"max") == 0 ? -1 : _wtoi(Match[3].str().c_str())
		: 0;

	if (::_wcsicmp(Match[1].str().c_str(), L"TinyInt") == 0)	return EDatabaseDataType::TinyInt;
	if (::_wcsicmp(Match[1].str().c_str(), L"SmallInt") == 0)	return EDatabaseDataType::SmallInt;
	if (::_wcsicmp(Match[1].str().c_str(), L"Int") == 0)		return EDatabaseDataType::Int;
	if (::_wcsicmp(Match[1].str().c_str(), L"Real") == 0)		return EDatabaseDataType::Real;
	if (::_wcsicmp(Match[1].str().c_str(), L"DateTime") == 0)	return EDatabaseDataType::DateTime;
	if (::_wcsicmp(Match[1].str().c_str(), L"Float") == 0)		return EDatabaseDataType::Float;
	if (::_wcsicmp(Match[1].str().c_str(), L"Bit") == 0)		return EDatabaseDataType::Bit;
	if (::_wcsicmp(Match[1].str().c_str(), L"Numeric") == 0)	return EDatabaseDataType::Numeric;
	if (::_wcsicmp(Match[1].str().c_str(), L"BigInt") == 0)		return EDatabaseDataType::BigInt;
	if (::_wcsicmp(Match[1].str().c_str(), L"VarBinary") == 0)	return EDatabaseDataType::VarBinary;
	if (::_wcsicmp(Match[1].str().c_str(), L"Varchar") == 0)	return EDatabaseDataType::Varchar;
	if (::_wcsicmp(Match[1].str().c_str(), L"Binary") == 0)		return EDatabaseDataType::Binary;
	if (::_wcsicmp(Match[1].str().c_str(), L"NVarChar") == 0)	return EDatabaseDataType::NVarChar;

	return EDatabaseDataType::None;
}
