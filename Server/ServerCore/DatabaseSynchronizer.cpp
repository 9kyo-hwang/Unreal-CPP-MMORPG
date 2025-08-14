#include "pch.h"
#include "DatabaseSynchronizer.h"

#include <ranges>
#include <regex>

#include "DataBaseBinder.h"
#include "DataBaseModel.h"
#include "RapidXml.h"

namespace SP
{
	const TCHAR* QTablesAndColumns =
		L"	SELECT c.object_id, t.name AS tableName, c.name AS columnName, c.column_id, c.user_type_id, c.max_length,"
		"		c.is_nullable, c.is_identity, CAST(ic.seed_value AS BIGINT) AS seedValue, CAST(ic.increment_value AS BIGINT) AS incValue,"
		"		c.default_object_id, dc.definition as defaultDefinition, dc.name as defaultConstraintName"
		"	FROM sys.columns AS c"
		"	JOIN sys.tables AS t"
		"		ON c.object_id = t.object_id"
		"	LEFT JOIN sys.default_constraints AS dc"
		"		ON c.default_object_id = dc.object_id"
		"	LEFT JOIN sys.identity_columns AS ic"
		"		ON c.object_id = ic.object_id AND c.column_id = ic.column_id"
		"	WHERE t.type = 'U'"
		"	ORDER BY object_id ASC, column_id ASC;";

	class FDatabaseTables : public FDatabaseBinder<0, 13>
	{
	public:
		FDatabaseTables(FDatabaseConnection& Connection) : FDatabaseBinder(Connection, QTablesAndColumns) {}

		void GetObjectId(int32& OutId) { BindCol(0, OutId); }
		template<int32 N> void GetTableName(TCHAR(&value)[N]) { BindCol(1, value); }
		template<int32 N> void GetColumnName(TCHAR(&value)[N]) { BindCol(2, value); }
		void GetColumnId(int32& OutId) { BindCol(3, OutId); }
		void GetUserType(int32& OutType) { BindCol(4, OutType); }
		void GetMaxLength(int32& OutLength) { BindCol(5, OutLength); }
		void GetIsNullable(bool& bOutNullable) { BindCol(6, bOutNullable); }
		void GetIsIdentity(OUT bool& bOutIdentity) { BindCol(7, bOutIdentity); }
		void GetSeedValue(int64& OutSeed) { BindCol(8, OutSeed); }
		void GetIncrementValue(int64& OutIncrement) { BindCol(9, OutIncrement); }
		void GetDefaultObjectId(int32& OutDefaultId) { BindCol(10, OutDefaultId); }
		template<int32 N> void GetDefaultDefinition(TCHAR(&OutDefaultDefinition)[N]) { BindCol(11, OutDefaultDefinition); }
		template<int32 N> void GetDefaultConstraintName(TCHAR(&OutDefaultConstraintName)[N]) { BindCol(12, OutDefaultConstraintName); }
	};

	const TCHAR* QIndexes =
		L"	SELECT i.object_id, i.name as indexName, i.index_id, i.type, i.is_primary_key,"
		"		i.is_unique_constraint, ic.column_id, COL_NAME(ic.object_id, ic.column_id) as columnName"
		"	FROM sys.indexes AS i"
		"	JOIN sys.index_columns AS ic"
		"		ON i.object_id = ic.object_id AND i.index_id = ic.index_id"
		"	WHERE i.type > 0 AND i.object_id IN(SELECT object_id FROM sys.tables WHERE type = 'U')"
		"	ORDER BY i.object_id ASC, i.index_id ASC;";

	class FDatabaseIndices : public FDatabaseBinder<0, 8>
	{
	public:
		FDatabaseIndices(FDatabaseConnection& Connection) : FDatabaseBinder(Connection, QIndexes) {}

		void GetObjectId(int32& OutId) { BindCol(0, OutId); }
		template<int32 N> void GetIndexName(TCHAR(&OutName)[N]) { BindCol(1, OutName); }
		void GetIndexId(int32& OutId) { BindCol(2, OutId); }
		void GetIndexType(int32& OutType) { BindCol(3, OutType); }
		void GetIsPrimaryKey(bool& bOutPrimaryKey) { BindCol(4, bOutPrimaryKey); }
		void GetIsUniqueConstraint(bool& bOutUniqueConstraint) { BindCol(5, bOutUniqueConstraint); }
		void GetColumnId(int32& OutId) { BindCol(6, OutId); }
		template<int32 N> void GetColumnName(TCHAR(&OutColumnName)[N]) { BindCol(7, OutColumnName); }
	};

	const TCHAR* QStoredProcedures =
		L"	SELECT name, OBJECT_DEFINITION(object_id) AS body FROM sys.procedures;";

	class FDatabaseStoredProcedures : public FDatabaseBinder<0, 2>
	{
	public:
		FDatabaseStoredProcedures(FDatabaseConnection& Connection) : FDatabaseBinder(Connection, QStoredProcedures) {}

		template<int32 N> void GetName(TCHAR(&OutName)[N]) { BindCol(0, OutName); }
		void GetBody(TCHAR* value, int32 len) { BindCol(1, value, len); }
	};
}

FDatabaseSynchronizer::FDatabaseSynchronizer(FDatabaseConnection& InConnection)
	: Connection(InConnection)
{
}

FDatabaseSynchronizer::~FDatabaseSynchronizer()
{
}

bool FDatabaseSynchronizer::Synchronize(const TCHAR* XmlFilePath)
{
	ParseXml(XmlFilePath);

	if (!GatherTables())
	{
		wcout << TEXT("FAILED [Gather Tables]") << endl;
		return false;
	}

	if (!GatherIndices())
	{
		wcout << TEXT("FAILED [Gather Indices]") << endl;
		return false;
	}

	if (GatherStoredProcedures())
	{
		wcout << TEXT("FAILED [Gather Stored Procedures]") << endl;
		return false;
	}

	CompareModel();
	ExecuteUpdateQueries();

	return true;
}

void FDatabaseSynchronizer::ParseXml(const TCHAR* XmlFilePath)
{
	FXmlNode Root;
	FRapidXml Parser;
	check(Parser.ParseXmlFile(XmlFilePath, Root));

	for (const FXmlNode& XmlTable : Root.GetChildrenNodes(TEXT("Table")))
	{
		auto Table = MakeShared<FDatabaseTableInfo>();
		Table->Name = XmlTable.GetAttribute(TEXT("name"), TEXT(""));

		for (const FXmlNode& XmlColumn : XmlTable.GetChildrenNodes(TEXT("Column")))
		{
			auto Column = MakeShared<FDatabaseColumnInfo>();
			
			Column->ColumnName = XmlColumn.GetAttribute(TEXT("name"), TEXT(""));
			Column->DataTypeText = XmlColumn.GetAttribute(TEXT("type"), TEXT(""));
			Column->bNullable = XmlColumn.GetAttribute(TEXT("notnull"), false) == false;

			Column->DataType = FDatabaseHelpers::String2DataType(Column->DataTypeText.c_str(), Column->MaxLen);
			check(Column->DataType != EDatabaseDataType::None);

			const TCHAR* Identity = XmlColumn.GetAttribute(TEXT("identity"), TEXT(""));
			if (::wcslen(Identity) > 0)
			{
				std::wregex Pattern(L"(\\d+),(\\d+)");
				std::wcmatch Match;
				check(std::regex_match(Identity, Match, Pattern));

				Column->bIdentity = true;
				Column->Seed = _wtoi(Match[1].str().c_str());
				Column->Increment = _wtoi(Match[2].str().c_str());
			}

			Column->Default = XmlColumn.GetAttribute(TEXT("default"), TEXT(""));
			Table->Columns.push_back(Column);
		}

		for (const FXmlNode& XmlIndex : XmlTable.GetChildrenNodes(TEXT("Index")))
		{
			shared_ptr<FDatabaseIndexInfo> Index = MakeShared<FDatabaseIndexInfo>();
			
			const TCHAR* IndexType = XmlIndex.GetAttribute(TEXT("type"), TEXT(""));
			if (::_wcsicmp(IndexType, TEXT("clustered")) == 0)
			{
				Index->IndexType = EDatabaseIndexType::Clustered;
			}
			else if (::_wcsicmp(IndexType, TEXT("nonclustered")) == 0)
			{
				Index->IndexType = EDatabaseIndexType::NonClustered;
			}
			else
			{
				CRASH("Invalid Index Type");
			}

			Index->bPrimaryKey = XmlIndex.FindChildNode(TEXT("PrimaryKey")).IsValid();
			Index->bUniqueConstraint = XmlIndex.FindChildNode(TEXT("UniqueKey")).IsValid();

			for (const FXmlNode& IndexColumn : XmlIndex.GetChildrenNodes(TEXT("Column")))
			{
				const TCHAR* ColumnName = IndexColumn.GetAttribute(TEXT("name"), TEXT(""));
				shared_ptr<FDatabaseColumnInfo> Column = Table->GetColumn(ColumnName);
				check(Column != nullptr);
				Index->Columns.push_back(Column);
			}

			Table->Indices.push_back(Index);
		}

		XmlTables.push_back(Table);
	}

	for (const FXmlNode& XmlProcedure : Root.GetChildrenNodes(TEXT("Procedure")))
	{
		auto Procedure = MakeShared<FDatabaseProcedureInfo>();
		Procedure->Name = XmlProcedure.GetAttribute(TEXT("name"), TEXT(""));
		Procedure->Body = XmlProcedure.FindChildNode(TEXT("Body")).GetContent(TEXT(""));

		for (const FXmlNode& XmlParam : XmlProcedure.GetChildrenNodes(TEXT("Param")))
		{
			FDatabaseProcedureParam Param;
			Param.Name = XmlParam.GetAttribute(TEXT("name"), TEXT(""));
			Param.Type = XmlParam.GetAttribute(TEXT("type"), TEXT(""));

			Procedure->Params.push_back(Param);
		}

		XmlProcedures.push_back(Procedure);
	}

	for (const FXmlNode& XmlRemovedTable : Root.GetChildrenNodes(TEXT("RemovedTable")))
	{
		XmlRemovedTables.insert(XmlRemovedTable.GetAttribute(TEXT("name"), TEXT("")));
	}
}

bool FDatabaseSynchronizer::GatherTables()
{
	int32 ObjectId;
	TCHAR TableName[101] = { 0 };
	TCHAR ColumnName[101] = { 0 };
	int32 ColumnId;
	int32 UserTypeId;
	int32 MaxLength;
	bool bIsNullable;
	bool bIsIdentity;
	int64 Seed;
	int64 Increment;
	int32 DefaultObjectId;
	TCHAR DefaultDefinition[101] = { 0 };
	TCHAR DefaultConstraintName[101] = { 0 };

	SP::FDatabaseTables TablesGetter(Connection);
	TablesGetter.GetObjectId(ObjectId);
	TablesGetter.GetTableName(TableName);
	TablesGetter.GetColumnName(ColumnName);
	TablesGetter.GetColumnId(ColumnId);
	TablesGetter.GetUserType(UserTypeId);
	TablesGetter.GetMaxLength(MaxLength);
	TablesGetter.GetIsNullable(bIsNullable);
	TablesGetter.GetIsIdentity(bIsIdentity);
	TablesGetter.GetSeedValue(Seed);
	TablesGetter.GetIncrementValue(Increment);
	TablesGetter.GetDefaultObjectId(DefaultObjectId);
	TablesGetter.GetDefaultDefinition(DefaultDefinition);
	TablesGetter.GetDefaultConstraintName(DefaultConstraintName);

	if (TablesGetter.Execute() == false)
	{
		return false;
	}

	while (TablesGetter.Fetch())
	{
		shared_ptr<FDatabaseTableInfo> Table;

		auto TargetTable = ranges::find_if(DatabaseTables, [=](const shared_ptr<FDatabaseTableInfo>& InTable) { return InTable->ObjectId == ObjectId; });
		if (TargetTable == DatabaseTables.end())
		{
			Table = MakeShared<FDatabaseTableInfo>();
			Table->ObjectId = ObjectId;
			Table->Name = TableName;
			DatabaseTables .push_back(Table);
		}
		else
		{
			Table = *TargetTable;
		}

		auto Column = MakeShared<FDatabaseColumnInfo>();
		{
			Column->ColumnName = ColumnName;
			Column->ColumnId = ColumnId;
			Column->DataType = static_cast<EDatabaseDataType>(UserTypeId);
			Column->DataTypeText = FDatabaseHelpers::DataType2String(Column->DataType);
			Column->MaxLen = (Column->DataType == EDatabaseDataType::NVarChar ? MaxLength / 2 : MaxLength);
			Column->bNullable = bIsNullable;
			Column->bIdentity = bIsIdentity;
			Column->Seed = (bIsIdentity ? Seed : 0);
			Column->Increment = (bIsIdentity ? Increment : 0);

			if (DefaultObjectId > 0)
			{
				Column->Default = DefaultDefinition;
				uint64 p = Column->Default.find_first_not_of('(');
				Column->Default = Column->Default.substr(p, Column->Default.size() - p * 2);
				Column->DefaultConstraintName = DefaultConstraintName;
			}
		}

		Table->Columns.push_back(Column);
	}

	return true;
}

bool FDatabaseSynchronizer::GatherIndices()
{
	int32 ObjectId;
	TCHAR IndexName[101] = { 0 };
	int32 IndexId;
	int32 IndexType;
	bool bIsPrimaryKey;
	bool bIsUniqueConstraint;
	int32 ColumnId;
	TCHAR ColumnName[101] = { 0 };

	SP::FDatabaseIndices IndicesGetter(Connection);
	IndicesGetter.GetObjectId(ObjectId);
	IndicesGetter.GetIndexName(IndexName);
	IndicesGetter.GetIndexId(IndexId);
	IndicesGetter.GetIndexType(IndexType);
	IndicesGetter.GetIsPrimaryKey(bIsPrimaryKey);
	IndicesGetter.GetIsUniqueConstraint(bIsUniqueConstraint);
	IndicesGetter.GetColumnId(ColumnId);
	IndicesGetter.GetColumnName(ColumnName);

	if (IndicesGetter.Execute() == false)
	{
		return false;
	}

	while (IndicesGetter.Fetch())
	{
		auto TargetTable = ranges::find_if(DatabaseTables, [=](const shared_ptr<FDatabaseTableInfo>& InTable) { return InTable->ObjectId == ObjectId; });
		check(TargetTable != DatabaseTables.end());

		auto& Indices = (*TargetTable)->Indices;
		auto TargetIndex = ranges::find_if(Indices, [IndexId](const shared_ptr<FDatabaseIndexInfo>& InIndex) { return InIndex->IndexId == IndexId; });
		if (TargetIndex == Indices.end())
		{
			auto Index = MakeShared<FDatabaseIndexInfo>();
			{
				Index->IndexName = IndexName;
				Index->IndexId = IndexId;
				Index->IndexType = static_cast<EDatabaseIndexType>(IndexType);
				Index->bPrimaryKey = bIsPrimaryKey;
				Index->bUniqueConstraint = bIsUniqueConstraint;
			}

			Indices.push_back(Index);
			TargetIndex = Indices.end() - 1;
		}

		// 인덱스가 걸린 column 찾아서 매핑해준다.
		auto& Columns = (*TargetTable)->Columns;
		auto TargetColumn = ranges::find_if(Columns, [ColumnId](const shared_ptr<FDatabaseColumnInfo>& InColumn) { return InColumn->ColumnId == ColumnId; });
		check(TargetColumn != Columns.end());

		(*TargetIndex)->Columns.push_back(*TargetColumn);
	}

	return true;
}

bool FDatabaseSynchronizer::GatherStoredProcedures()
{
	TCHAR Name[101]{};
	TArray<TCHAR> Body(MaxProcedureLen);

	SP::FDatabaseStoredProcedures StoredProceduresGetter(Connection);
	StoredProceduresGetter.GetName(Name);
	StoredProceduresGetter.GetBody(&Body[0], MaxProcedureLen);

	if (StoredProceduresGetter.Execute() == false)
	{
		return false;
	}

	while (StoredProceduresGetter.Fetch())
	{
		auto Procedure = MakeShared<FDatabaseProcedureInfo>();
		{
			Procedure->Name = Name;
			Procedure->FullBody = FString(Body.begin(), ranges::find(Body, 0));
		}

		DatabaseProcedures.push_back(Procedure);
	}

	return true;
}

void FDatabaseSynchronizer::CompareModel()
{
	// 업데이트 목록 초기화.
	DependentIndices.clear();
	for (TArray<FString>& Queries : UpdateQueries)
	{
		Queries.clear();
	}

	// XML에 있는 목록을 우선 갖고 온다.
	TMap<FString, shared_ptr<FDatabaseTableInfo>> XmlTableMap;
	for (const auto& XmlTable : XmlTables)
	{
		XmlTableMap[XmlTable->Name] = XmlTable;
	}

	// DB에 실존하는 테이블들을 돌면서 XML에 정의된 테이블들과 비교한다.
	for (auto& DatabaseTable : DatabaseTables)
	{
		auto TargetTable = XmlTableMap.find(DatabaseTable->Name);
		if (TargetTable != XmlTableMap.end())
		{
			auto XmlTable = TargetTable->second;
			CompareTables(DatabaseTable, XmlTable);
			XmlTableMap.erase(TargetTable);
		}
		else
		{
			if (XmlRemovedTables.contains(DatabaseTable->Name))
			{
				GDebug->Log(EColor::Yellow, TEXT("Removing Table : [dbo].[%s]\n"), DatabaseTable->Name.c_str());
				UpdateQueries[EUpdateSteps::DropTable].push_back(FDatabaseHelpers::Format(TEXT("DROP TABLE [dbo].[%s]"), DatabaseTable->Name.c_str()));
			}
		}
	}

	// 맵에서 제거되지 않은 XML 테이블 정의는 새로 추가.
	for (auto& XmlTable : XmlTableMap | views::values)
	{
		FString ColumnsString;
		const int32 size = static_cast<int32>(XmlTable->Columns.size());
		for (int32 i = 0; i < size; i++)
		{
			if (i != 0)
			{
				ColumnsString += L",";
			}

			ColumnsString += L"\n\t";
			ColumnsString += XmlTable->Columns[i]->CreateText();
		}

		GDebug->Log(EColor::Yellow, TEXT("Creating Table : [dbo].[%s]\n"), XmlTable->Name.c_str());
		UpdateQueries[EUpdateSteps::CreateTable].push_back(FDatabaseHelpers::Format(TEXT("CREATE TABLE [dbo].[%s] (%s)"), XmlTable->Name.c_str(), ColumnsString.c_str()));

		for (const auto& XmlColumn : XmlTable->Columns)
		{
			if (XmlColumn->Default.empty())
			{
				continue;
			}

			UpdateQueries[EUpdateSteps::DefaultConstraint].push_back(
				FDatabaseHelpers::Format(
					TEXT("ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] DEFAULT (%s) FOR [%s]"),
					XmlTable->Name.c_str(),
					FDatabaseHelpers::Format(L"DF_%s_%s", XmlTable->Name.c_str(), XmlColumn->ColumnName.c_str()).c_str(),
					XmlColumn->Default.c_str(),
					XmlColumn->ColumnName.c_str())
			);
		}

		for (const auto& XmlIndex : XmlTable->Indices)
		{
			GDebug->Log(
				EColor::Yellow, 
				TEXT("Creating Index : [%s] %s %s [%s]\n"), 
				XmlTable->Name.c_str(),
				XmlIndex->GetKeyText().c_str(), 
				XmlIndex->GetTypeText().c_str(), 
				XmlIndex->GetUniqueName().c_str()
			);

			if (XmlIndex->bPrimaryKey || XmlIndex->bUniqueConstraint)
			{
				UpdateQueries[EUpdateSteps::CreateIndex].push_back(FDatabaseHelpers::Format(
					L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
					XmlTable->Name.c_str(),
					XmlIndex->CreateName(XmlTable->Name).c_str(),
					XmlIndex->GetKeyText().c_str(),
					XmlIndex->GetTypeText().c_str(),
					XmlIndex->CreateColumnsText().c_str()));
			}
			else
			{
				UpdateQueries[EUpdateSteps::CreateIndex].push_back(FDatabaseHelpers::Format(
					L"CREATE %s INDEX [%s] ON [dbo].[%s] (%s)",
					XmlIndex->GetTypeText().c_str(),
					XmlIndex->CreateName(XmlTable->Name).c_str(),
					XmlTable->Name.c_str(),
					XmlIndex->CreateColumnsText().c_str()));
			}
		}
	}

	CompareStoredProcedures();
}

void FDatabaseSynchronizer::CompareTables(shared_ptr<FDatabaseTableInfo> InTable, shared_ptr<FDatabaseTableInfo> InXmlTable)
{
	// XML에 있는 컬럼 목록을 갖고 온다.
	TMap<FString, shared_ptr<FDatabaseColumnInfo>> XmlColumnMap;
	for (auto& XmlColumn : InXmlTable->Columns)
	{
		XmlColumnMap[XmlColumn->ColumnName] = XmlColumn;
	}

	// DB에 실존하는 테이블 컬럼들을 돌면서 XML에 정의된 컬럼들과 비교한다.
	for (const auto& DatabaseColumn : InTable->Columns)
	{
		auto TargetColumn = XmlColumnMap.find(DatabaseColumn->ColumnName);
		if (TargetColumn != XmlColumnMap.end())
		{
			auto& XmlColumn = TargetColumn->second;
			CompareColumns(InTable, DatabaseColumn, XmlColumn);
			XmlColumnMap.erase(TargetColumn);
		}
		else
		{
			GDebug->Log(EColor::Yellow, TEXT("Dropping Column : [%s].[%s]\n"), InTable->Name.c_str(), DatabaseColumn->ColumnName.c_str());
			if (DatabaseColumn->DefaultConstraintName.empty() == false)
			{
				UpdateQueries[EUpdateSteps::DropColumn].push_back(
					FDatabaseHelpers::Format(
						TEXT("ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]"), 
						InTable->Name.c_str(), 
						DatabaseColumn->DefaultConstraintName.c_str()
					)
				);
			}

			UpdateQueries[EUpdateSteps::DropColumn].push_back(
				FDatabaseHelpers::Format(
					L"ALTER TABLE [dbo].[%s] DROP COLUMN [%s]", 
					InTable->Name.c_str(), 
					DatabaseColumn->ColumnName.c_str()
				)
			);
		}
	}

	// 맵에서 제거되지 않은 XML 컬럼 정의는 새로 추가.
	for (auto& XmlColumn : XmlColumnMap | views::values)
	{
		auto NewColumn = *XmlColumn;
		NewColumn.bNullable = true;

		GDebug->Log(
			EColor::Yellow, 
			TEXT("Adding Column : [%s].[%s]\n"), 
			InTable->Name.c_str(), 
			XmlColumn->ColumnName.c_str()
		);

		UpdateQueries[EUpdateSteps::AddColumn].push_back(
			FDatabaseHelpers::Format(
				TEXT("ALTER TABLE [dbo].[%s] ADD %s %s"),
				InTable->Name.c_str(), 
				XmlColumn->ColumnName.c_str(), 
				XmlColumn->DataTypeText.c_str()
			)
		);

		if (XmlColumn->bNullable == false && XmlColumn->Default.empty() == false)
		{
			UpdateQueries[EUpdateSteps::AddColumn].push_back(
				FDatabaseHelpers::Format(
					TEXT("SET NOCOUNT ON; UPDATE [dbo].[%s] SET [%s] = %s WHERE [%s] IS NULL"),
					InTable->Name.c_str(),
					XmlColumn->ColumnName.c_str(),
					XmlColumn->ColumnName.c_str(),
					XmlColumn->ColumnName.c_str()
				)
			);
		}

		if (XmlColumn->bNullable == false)
		{
			UpdateQueries[EUpdateSteps::AddColumn].push_back(
				FDatabaseHelpers::Format(
					L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
					InTable->Name.c_str(), 
					XmlColumn->CreateText().c_str()
				)
			);
		}

		if (XmlColumn->Default.empty() == false)
		{
			UpdateQueries[EUpdateSteps::AddColumn].push_back(
				FDatabaseHelpers::Format(
					L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (%s) FOR [%s]",
					InTable->Name.c_str(), 
					InTable->Name.c_str(), 
					XmlColumn->ColumnName.c_str(), 
					XmlColumn->Default.c_str(), 
					XmlColumn->ColumnName.c_str()
				)
			);
		}
	}

	// XML에 있는 인덱스 목록을 갖고 온다.
	TMap<FString, shared_ptr<FDatabaseIndexInfo>> XmlIndexMap;
	for (auto& XmlIndex : InXmlTable->Indices)
	{
		XmlIndexMap[XmlIndex->GetUniqueName()] = XmlIndex;
	}

	// DB에 실존하는 테이블 인덱스들을 돌면서 XML에 정의된 인덱스들과 비교한다.
	for (auto& DatabaseIndex : InTable->Indices)
	{
		auto TargetIndex = XmlIndexMap.find(DatabaseIndex->GetUniqueName());
		if (TargetIndex != XmlIndexMap.end() && !DependentIndices.contains(DatabaseIndex->GetUniqueName()))
		{
			auto XmlIndex = TargetIndex->second;
			XmlIndexMap.erase(TargetIndex);
		}
		else
		{
			GDebug->Log(
				EColor::Yellow, 
				L"Dropping Index : [%s] [%s] %s %s\n", 
				InTable->Name.c_str(), 
				DatabaseIndex->IndexName.c_str(), 
				DatabaseIndex->GetKeyText().c_str(), 
				DatabaseIndex->GetTypeText().c_str()
			);

			if (DatabaseIndex->bPrimaryKey || DatabaseIndex->bUniqueConstraint)
			{
				UpdateQueries[EUpdateSteps::DropIndex].push_back(
					FDatabaseHelpers::Format(
						L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]", 
						InTable->Name.c_str(), 
						DatabaseIndex->IndexName.c_str()
					)
				);
			}
			else
			{
				UpdateQueries[EUpdateSteps::DropIndex].push_back(
					FDatabaseHelpers::Format(
						L"DROP INDEX [%s] ON [dbo].[%s]", 
						DatabaseIndex->IndexName.c_str(), 
						InTable->Name.c_str()
					)
				);
			}
		}
	}

	// 맵에서 제거되지 않은 XML 인덱스 정의는 새로 추가.
	for (auto& XmlIndex : XmlIndexMap | views::values)
	{
		GDebug->Log(
			EColor::Yellow, 
			L"Creating Index : [%s] %s %s [%s]\n", 
			InTable->Name.c_str(), 
			XmlIndex->GetKeyText().c_str(), 
			XmlIndex->GetTypeText().c_str(), 
			XmlIndex->GetUniqueName().c_str()
		);

		if (XmlIndex->bPrimaryKey || XmlIndex->bUniqueConstraint)
		{
			UpdateQueries[EUpdateSteps::CreateIndex].push_back(
				FDatabaseHelpers::Format(
					L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
					InTable->Name.c_str(), 
					XmlIndex->CreateName(InTable->Name).c_str(), 
					XmlIndex->GetKeyText().c_str(), 
					XmlIndex->GetTypeText().c_str(), 
					XmlIndex->CreateColumnsText().c_str()
				)
			);
		}
		else
		{
			UpdateQueries[EUpdateSteps::CreateIndex].push_back(
				FDatabaseHelpers::Format(
					L"CREATE %s INDEX [%s] ON [dbo].[%s] (%s)",
					XmlIndex->GetTypeText().c_str(),
					XmlIndex->CreateName(InTable->Name).c_str(),
					InTable->Name.c_str(),
					XmlIndex->CreateColumnsText().c_str()
				)
			);
		}
	}
}

void FDatabaseSynchronizer::CompareColumns(shared_ptr<FDatabaseTableInfo> InTable, shared_ptr<FDatabaseColumnInfo> InColumn,
	shared_ptr<FDatabaseColumnInfo> InXmlColumn)
{
	uint8 Flag = 0;

	if (InColumn->DataType != InXmlColumn->DataType)
	{
		Flag |= EColumnFlags::Type;
	}

	if (InColumn->MaxLen != InXmlColumn->MaxLen && InXmlColumn->MaxLen > 0)
	{
		Flag |= EColumnFlags::Length;
	}
	if (InColumn->bNullable != InXmlColumn->bNullable)
	{
		Flag |= EColumnFlags::Nullable;
	}

	if (InColumn->bIdentity != InXmlColumn->bIdentity || (InColumn->bIdentity && InColumn->Increment != InXmlColumn->Increment))
	{
		Flag |= EColumnFlags::Identity;
	}

	if (InColumn->Default != InXmlColumn->Default)
	{
		Flag |= EColumnFlags::Default;
	}

	if (Flag)
	{
		GDebug->Log(
			EColor::Yellow, 
			L"Updating Column [%s] : (%s) -> (%s)\n", 
			InTable->Name.c_str(), 
			InColumn->CreateText().c_str(), 
			InXmlColumn->CreateText().c_str()
		);
	}

	// 연관된 인덱스가 있으면 나중에 삭제하기 위해 기록한다.
	if (Flag & (EColumnFlags::Type | EColumnFlags::Length | EColumnFlags::Nullable))
	{
		for (const auto& Index : InTable->Indices)
		{
			if (Index->DependsOn(InColumn->ColumnName))
			{
				DependentIndices.insert(Index->GetUniqueName());
			}
		}

		Flag |= EColumnFlags::Default;
	}

	if (Flag & EColumnFlags::Default)
	{
		if (InColumn->DefaultConstraintName.empty() == false)
		{
			UpdateQueries[EUpdateSteps::AlterColumn].push_back(
				FDatabaseHelpers::Format(
					L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]",
					InTable->Name.c_str(),
					InColumn->DefaultConstraintName.c_str()
				)
			);
		}
	}

	auto NewColumn = *InColumn;
	NewColumn.Default = L"";
	NewColumn.DataType = InXmlColumn->DataType;
	NewColumn.MaxLen = InXmlColumn->MaxLen;
	NewColumn.DataTypeText = InXmlColumn->DataTypeText;
	NewColumn.Seed = InXmlColumn->Seed;
	NewColumn.Increment = InXmlColumn->Increment;

	if (Flag & (EColumnFlags::Type | EColumnFlags::Length | EColumnFlags::Identity))
	{
		UpdateQueries[EUpdateSteps::AlterColumn].push_back(
			FDatabaseHelpers::Format(
				L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
				InTable->Name.c_str(),
				NewColumn.CreateText().c_str()
			)
		);
	}

	NewColumn.bNullable = InXmlColumn->bNullable;
	if (Flag & EColumnFlags::Nullable)
	{
		if (InXmlColumn->Default.empty() == false)
		{
			UpdateQueries[EUpdateSteps::AlterColumn].push_back(
				FDatabaseHelpers::Format(
					L"SET NOCOUNT ON; UPDATE [dbo].[%s] SET [%s] = %s WHERE [%s] IS NULL",
					InTable->Name.c_str(),
					InXmlColumn->ColumnName.c_str(),
					InXmlColumn->Default.c_str(),
					InXmlColumn->ColumnName.c_str()));
		}

		UpdateQueries[EUpdateSteps::AlterColumn].push_back(
			FDatabaseHelpers::Format(
				L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
				InTable->Name.c_str(),
				NewColumn.CreateText().c_str()
			)
		);
	}

	if (Flag & EColumnFlags::Default)
	{
		if (InColumn->DefaultConstraintName.empty() == false)
		{
			UpdateQueries[EUpdateSteps::AlterColumn].push_back(
				FDatabaseHelpers::Format(
					L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] DEFAULT (%s) FOR [%s]",
					InTable->Name.c_str(),
					FDatabaseHelpers::Format(
						L"DF_%s_%s", 
						InTable->Name.c_str(), 
						InColumn->ColumnName.c_str()
					).c_str(),
					InColumn->Default.c_str(), InColumn->ColumnName.c_str()
				)
			);
		}
	}
}

void FDatabaseSynchronizer::CompareStoredProcedures()
{
	// XML에 있는 프로시저 목록을 갖고 온다.
	TMap<FString, shared_ptr<FDatabaseProcedureInfo>> XmlProcedureMap;
	for (auto& XmlProcedure : XmlProcedures)
	{
		XmlProcedureMap[XmlProcedure->Name] = XmlProcedure;
	}

	// DB에 실존하는 테이블 프로시저들을 돌면서 XML에 정의된 프로시저들과 비교한다.
	for (const auto& Procedure : DatabaseProcedures)
	{
		auto TargetProcedure = XmlProcedureMap.find(Procedure->Name);
		if (TargetProcedure != XmlProcedureMap.end())
		{
			auto XmlProcedure = TargetProcedure->second;
			FString XmlBody = XmlProcedure->GenerateCreateQuery();
			if (FDatabaseHelpers::RemoveWhiteSpace(Procedure->FullBody) != FDatabaseHelpers::RemoveWhiteSpace(XmlBody))
			{
				GDebug->Log(
					EColor::Yellow, 
					L"Updating Procedure : %s\n", 
					Procedure->Name.c_str()
				);

				UpdateQueries[EUpdateSteps::StoredProcedure].push_back(
					XmlProcedure->GenerateAlterQuery()
				);
			}

			XmlProcedureMap.erase(TargetProcedure);
		}
	}

	// 맵에서 제거되지 않은 XML 프로시저 정의는 새로 추가.
	for (auto& [ProcedureName, Procedure] : XmlProcedureMap)
	{
		GDebug->Log(EColor::Yellow, 
			L"Updating Procedure : %s\n", 
			ProcedureName.c_str()
		);

		UpdateQueries[EUpdateSteps::StoredProcedure].push_back(
			Procedure->GenerateCreateQuery()
		);
	}
}

void FDatabaseSynchronizer::ExecuteUpdateQueries()
{
	for (int32 Step = 0; Step < EUpdateSteps::Max; Step++)
	{
		for (FString& Query : UpdateQueries[Step])
		{
			Connection.Unbind();
			if(Connection.Execute(Query.c_str()) == false)
			{
				wcout << TEXT("Query (Step: ") << Step << TEXT("): ") << Query << endl;
				CRASH("Query Execution Failed");
			}
		}
	}
}
