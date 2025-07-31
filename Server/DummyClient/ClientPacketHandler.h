#pragma once

enum EPacketFromServer
{
	Test = 1,
};

class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* Buffer, int32 Length);

private:
	static void HandleTestPacket(BYTE* Buffer, int32 Length);
};

template<typename ElementType, typename ContainerType>
class TPacketIterator
{
public:
	TPacketIterator(ContainerType& InContainer, uint16 InIndex)
		: Container(InContainer)
		, Index(InIndex)
	{}

	ElementType& operator*()
	{
		check(Index < Container.Num());
		return Container[Index];
	}

	const ElementType& operator*() const
	{
		check(Index < Container.Num());
		return Container[Index];
	}

	ElementType* operator->()
	{
		check(Index < Container.Num());
		return &Container[Index];
	}

	bool operator==(const TPacketIterator& Other) const
	{
		return Index == Other.Index;
	}

	bool operator!=(const TPacketIterator& Other) const
	{
		return !(*this == Other);
	}

	TPacketIterator& operator++()
	{
		check(Index < Container.Num());
		++Index;
		return *this;
	}

	TPacketIterator operator++(int32)
	{
		TPacketIterator Temp = *this;
		++Index;
		return Temp;
	}

private:
	ContainerType& Container;
	uint16 Index;
};

template<typename DataType>
class TPacketArray
{
public:
	TPacketArray()
		: Data(nullptr)
		, ArrayNum(0)
	{}

	TPacketArray(DataType* InData, uint16 InNum)
		: Data(InData)
		, ArrayNum(InNum)
	{}

	DataType& operator[](uint16 Index)
	{
		check(Index < ArrayNum);
		return Data[Index];
	}

	DataType* GetData() { return Data; }
	uint16 Num() const { return ArrayNum; }

	TPacketIterator<DataType, TPacketArray<DataType>> begin()
	{
		return TPacketIterator<DataType, TPacketArray>(*this, 0);
	}

	TPacketIterator<DataType, TPacketArray<DataType>> end()
	{
		return TPacketIterator<DataType, TPacketArray>(*this, ArrayNum);
	}

private:
	DataType* Data;	// 시작 주소
	uint16 ArrayNum;	// 현재 개수
};