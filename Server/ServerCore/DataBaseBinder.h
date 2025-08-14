#pragma once
#include "DataBaseConnection.h"

template<int32 Len>
struct FIsAllBitSet { enum { Value = 1 << Len - 1 | FIsAllBitSet<Len - 1>::Value }; };

template<>
struct FIsAllBitSet<1> { enum { Value = 1 }; };

template<>
struct FIsAllBitSet<0> { enum { Value = 0 }; };

// (NumParams, NumColumns) 조합마다 별개의 클래스로 생성됨
template<int32 NumParams, int32 NumColumns>
class FDatabaseBinder
{
public:
	FDatabaseBinder(FDatabaseConnection& InConnection, const TCHAR* InCommandString)
		: Connection(InConnection)
		, CommandString(InCommandString)
		, ParamIndPtrs{}
		, ColumnIndPtrs{}
		, ParamFlag(0)
		, ColumnFlag(0)
	{
		Connection.Unbind();
	}

	bool IsValid() { return ParamFlag == FIsAllBitSet<NumParams>::Value && ColumnFlag == FIsAllBitSet<NumColumns>::Value; }
	bool Execute() { check(IsValid()); return Connection.Execute(CommandString); }
	bool Fetch() const { return Connection.Fetch(); }

public:
	template<typename T>
	void BindParam(int32 Param, T& Value)
	{
		// 0번부터 넘겨받고, 내부적으로 + 1 처리
		Connection.BindParam(
			Param + 1, 
			&Value, 
			&ParamIndPtrs[Param]
		);

		ParamFlag |= 1LL << Param;	// 해당 번호 비트 ON
	}

	void BindParam(int32 Param, const TCHAR* Value)
	{
		Connection.BindParam(
			Param + 1, 
			Value, 
			&ParamIndPtrs[Param]
		);

		ParamFlag |= 1LL << Param;
	}

	template<typename T, int32 Num>
	void BindParam(int32 Param, T(&Value)[Num])
	{
		Connection.BindParam(
			Param + 1, 
			(const BYTE*)Value, 
			sizeof(T) * Num,
			&ParamIndPtrs[Param]
		);

		ParamFlag |= 1LL << Param;
	}

	template<typename T>
	void BindParam(int32 Param, T* Value, int32 Num)
	{
		Connection.BindParam(
			Param + 1,
			(const BYTE*)Value,
			sizeof(T) * Num,
			&ParamIndPtrs[Param]
		);

		ParamFlag |= 1LL << Param;
	}

	template<typename T>
	void BindCol(int32 Column, T& Value)
	{
		Connection.BindCol(
			Column + 1, 
			&Value, 
			&ColumnIndPtrs[Column]
		);

		ColumnFlag |= 1LL << Column;
	}

	template<int32 Num>
	void BindCol(int32 Column, TCHAR(&Value)[Num])
	{
		// null byte를 빼면 -1
		Connection.BindCol(
			Column + 1, 
			Value, 
			Num - 1, 
			&ColumnIndPtrs[Column]
		);

		ColumnFlag |= 1LL << Column;
	}

	void BindCol(int32 Column, TCHAR* Value, int32 Size)
	{
		Connection.BindCol(
			Column + 1, 
			Value, 
			Size - 1, 
			&ColumnIndPtrs[Column]
		);

		ColumnFlag |= 1LL << Column;
	}

	// 이미지 파일과 같은 바이트 배열
	template<typename T, int32 Num>
	void BindCol(int32 Column, T(&Value)[Num])
	{
		Connection.BindCol(
			Column + 1, 
			Value, 
			sizeof(T) * Num, 
			ColumnIndPtrs[Column]
		);

		ColumnFlag |= 1LL << Column;
	}

private:
	FDatabaseConnection& Connection;
	const TCHAR* CommandString;
	SQLLEN ParamIndPtrs[NumParams > 0 ? NumParams : 1];	// 템플릿 인자라 연산식도 컴파일 타임에 계산
	SQLLEN ColumnIndPtrs[NumColumns > 0 ? NumColumns : 1];
	uint64 ParamFlag;
	uint64 ColumnFlag;
};

