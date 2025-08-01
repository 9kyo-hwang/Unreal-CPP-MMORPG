#pragma once
#include "BufferWriter.h"

enum EPacketId
{
	Test = 1,
};

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* Buffer, int32 Length);
};

template<typename ElementType, typename ContainerType>
class TPacketIterator
{
public:
	TPacketIterator(ContainerType& InContainer, uint16 InIndex)
		: Container(InContainer)
		, Index(InIndex)
	{
	}

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
	{
	}

	TPacketArray(DataType* InData, uint16 InNum)
		: Data(InData)
		, ArrayNum(InNum)
	{
	}

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

#pragma pack(1)
// [ ServerPacket_Test ][ FBuffData FBuffData FBuffData ... ] [ Victim Victim ] [ Victim Victim ] ..
struct ServerPacket_Test
{
	struct FBuffData
	{
		uint64 BuffId;
		float RemainTime;

		// Victims List
		uint16 VictimsOffset;
		uint16 NumVictims;
	};

	uint16 PacketId;
	uint16 PacketSize;

	uint64 Id;
	uint32 Hp;
	uint16 Attack;

	uint16 BuffsOffset;	// 앞에 붙어있는 패킷만큼 건너뛰어야 FBuffData 배열이 시작됨
	uint16 NumBuff;
};

class ServerPacketWriter_Test
{
public:
	using FPacket = ServerPacket_Test;
	using FBuffData = FPacket::FBuffData;
	using FBuffDataArray = TPacketArray<FBuffData>;
	using FBuffDataVictimArray = TPacketArray<uint64>; // Victim Ids

	ServerPacketWriter_Test(uint64 Id, uint32 Hp, uint16 Attack);
	FBuffDataArray ReserveBuffs(uint16 NumBuff);
	FBuffDataVictimArray ReserveVictims(FBuffData* Buff, uint16 NumVictim);
	shared_ptr<FSendBuffer> Close();

private:
	FPacket* Packet;
	shared_ptr<FSendBuffer> SendBuffer;
	FBufferWriter Writer;
};

#pragma pack()