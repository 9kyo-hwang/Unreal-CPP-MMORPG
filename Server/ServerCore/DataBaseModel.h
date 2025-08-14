#pragma once

enum class EDatabaseDataType
{
	None = 0,
	TinyInt = 48,
	SmallInt = 52,
	Int = 56,
	Real = 59,
	DateTime = 61,
	Float = 62,
	Bit = 104,
	Numeric = 108,
	BigInt = 127,
	VarBinary = 165,
	Varchar = 167,
	Binary = 173,
	NVarChar = 231,
};

class FDatabaseColumnInfo
{
public:
	FDatabaseColumnInfo();

	FString CreateText() const;

	FString ColumnName;
	int32 ColumnId;					// Only DB
	EDatabaseDataType DataType;
	FString DataTypeText;
	int32 MaxLen;
	bool bNullable;
	bool bIdentity;
	int64 Seed;
	int64 Increment;
	FString Default;
	FString DefaultConstraintName;	// Only DB
};

enum class EDatabaseIndexType
{
	Clustered = 1,
	NonClustered = 2
};

class FDatabaseIndexInfo
{
public:
	FDatabaseIndexInfo();

	FString				GetUniqueName() const;
	FString				CreateName(const FString& Table) const;
	FString				GetTypeText() const;
	FString				GetKeyText() const;
	FString				CreateColumnsText() const;
	bool				DependsOn(const FString& Column);

	FString IndexName;			// DB Only
	int32 IndexId;			// DB ONly
	EDatabaseIndexType IndexType;
	bool bPrimaryKey;
	bool bUniqueConstraint;
	TArray<shared_ptr<FDatabaseColumnInfo>> Columns;
};

class FDatabaseTableInfo
{
public:
	FDatabaseTableInfo();

	shared_ptr<FDatabaseColumnInfo> GetColumn(const FString& ColumnName);

	int32 ObjectId;
	FString Name;
	TArray<shared_ptr<FDatabaseColumnInfo>> Columns;
	TArray<shared_ptr<FDatabaseIndexInfo>> Indices;
};

struct FDatabaseProcedureParam
{
	FString Name;
	FString Type;
};

class FDatabaseProcedureInfo
{
public:
	FString	GenerateCreateQuery() const;
	FString	GenerateAlterQuery() const;
	FString	GenerateParamString() const;

	FString Name;
	FString FullBody;						// Only DB
	FString Body;							// Only XML
	TArray<FDatabaseProcedureParam> Params;	// Only XML
};

class FDatabaseHelpers
{
public:
	static FString Format(const TCHAR* Format, ...);
	static FString DataType2String(EDatabaseDataType Type);
	static FString RemoveWhiteSpace(const FString& Str);
	static EDatabaseDataType String2DataType(const TCHAR* Str, int32& MaxLen);
};

