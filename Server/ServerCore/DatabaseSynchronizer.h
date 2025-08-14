#pragma once

class FDatabaseProcedureInfo;
class FDatabaseColumnInfo;
class FDatabaseTableInfo;
class FDatabaseConnection;

class FDatabaseSynchronizer
{
	static constexpr uint64 MaxProcedureLen = 10000;

	enum EUpdateSteps : uint8
	{
		DropIndex,
		AlterColumn,
		AddColumn,
		CreateTable,
		DefaultConstraint,
		CreateIndex,
		DropColumn,
		DropTable,
		StoredProcedure,

		Max
	};

	enum EColumnFlags : uint8
	{
		Type = 1 << 0,
		Nullable = 1 << 1,
		Identity = 1 << 2,
		Default = 1 << 3,
		Length = 1 << 4,
	};

public:
	FDatabaseSynchronizer(FDatabaseConnection& InConnection);
	~FDatabaseSynchronizer();

	bool Synchronize(const TCHAR* XmlFilePath);

private:
	void ParseXml(const TCHAR* XmlFilePath);
	bool GatherTables();
	bool GatherIndices();
	bool GatherStoredProcedures();

	void CompareModel();
	void CompareTables(shared_ptr<FDatabaseTableInfo> InTable, shared_ptr<FDatabaseTableInfo> InXmlTable);
	void CompareColumns(shared_ptr<FDatabaseTableInfo> InTable, shared_ptr<FDatabaseColumnInfo> InColumn, shared_ptr<FDatabaseColumnInfo> InXmlColumn);
	void CompareStoredProcedures();

	void ExecuteUpdateQueries();

private:
	FDatabaseConnection& Connection;

	TArray<shared_ptr<FDatabaseTableInfo>> XmlTables;
	TArray<shared_ptr<FDatabaseProcedureInfo>> XmlProcedures;
	TSet<FString> XmlRemovedTables;

	TArray<shared_ptr<FDatabaseTableInfo>> DatabaseTables;
	TArray<shared_ptr<FDatabaseProcedureInfo>> DatabaseProcedures;

	TSet<FString> DependentIndices;
	TArray<FString> UpdateQueries[EUpdateSteps::Max];
};

