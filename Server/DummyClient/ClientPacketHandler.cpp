#include "pch.h"
#include "ClientPacketHandler.h"

#include "BufferReader.h"
#include "Session.h"

void ClientPacketHandler::HandlePacket(BYTE* Buffer, int32 Length)
{
	FBufferReader br(Buffer, Length);
	FPacketHeader PacketHeader; br >> PacketHeader;

	switch (PacketHeader.Id)
	{
	case EPacketFromServer::Test:
		HandleTestPacket(Buffer, Length);
		break;
	default:
		break;
	}
}

#pragma pack(1)
// [ ServerPacket_Test ][ FBuffData FBuffData FBuffData ... ] 
struct ServerPacket_Test
{
	struct FBuffData
	{
		uint64 BuffId;
		float RemainTime;
	};

	uint16 PacketId;
	uint16 PacketSize;

	uint64 Id;
	uint32 Hp;
	uint16 Attack;

	uint16 BuffsOffset;	// 앞에 붙어있는 패킷만큼 건너뛰어야 FBuffData 배열이 시작됨
	uint16 NumBuff;

	bool IsValid() const
	{
		// 최소한 PacketHeader는 들어왔음이 보장, 패킷 크기가 구조체 크기보다 작다면 유효하지 않음
		if (PacketSize < sizeof(ServerPacket_Test))
		{
			return false;
		}

		// 패킷 헤더에 기록된 전체 크기가 실제로 지금 계산해본 크기와 일치하는지 확인
		if (sizeof(ServerPacket_Test) + NumBuff * sizeof(FBuffData) != PacketSize)
		{
			return false;
		}

		// 버프 리스트의 오프셋이 정상적인지 확인(오프셋 + 버프 리스트 길이가 패킷 전체 크기를 넘어가면 엉뚱한 오프셋)
		if (BuffsOffset + NumBuff * sizeof(FBuffData) > PacketSize)
		{
			return false;
		}

		return true;
	}

	using FBuffDataArray = TPacketArray<FBuffData>;
	FBuffDataArray GetBuffs()
	{
		BYTE* Data = reinterpret_cast<BYTE*>(this) + BuffsOffset;
		return FBuffDataArray(reinterpret_cast<FBuffData*>(Data), NumBuff);
	}
};
#pragma pack()

void ClientPacketHandler::HandleTestPacket(BYTE* Buffer, int32 Length)
{
	if (Length < sizeof(ServerPacket_Test))
	{
		return;
	}

	ServerPacket_Test* Packet = reinterpret_cast<ServerPacket_Test*>(Buffer);
	if (!Packet->IsValid())
	{
		return;
	}

	TPacketArray<ServerPacket_Test::FBuffData> Buffs = Packet->GetBuffs();
	printf("ID: %llu, HP: %u, ATK: %u, Buff Count: %u\n", Packet->Id, Packet->Hp, Packet->Attack, Buffs.Num());
	for (const auto& Buff : Buffs)
	{
		printf("\tBuff ID: %llu, Remain Time: %.2f\n", Buff.BuffId, Buff.RemainTime);
	}
}
