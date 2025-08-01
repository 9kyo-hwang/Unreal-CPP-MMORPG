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
// [ ServerPacket_Test ][ FBuffData FBuffData FBuffData ][ Victim Victim Victim ][ Victim ][ Victim Victim ]
struct ServerPacket_Test
{
	struct FBuffData
	{
		uint64 BuffId;
		float RemainTime;

		// Victims List
		uint16 VictimsOffset;
		uint16 NumVictims;

		bool IsValid(BYTE* Packet, uint16 PacketSize, uint32& OutSize) const
		{
			// 해당 가변 데이터 크기가 전체 패킷을 넘어가는가?
			if (VictimsOffset + NumVictims * sizeof(uint64) > PacketSize)
			{
				return false;
			}

			OutSize += NumVictims * sizeof(uint64);
			return true;
		}
	};

	uint16 PacketId;
	uint16 PacketSize;

	uint64 Id;
	uint32 Hp;
	uint16 Attack;

	uint16 BuffsOffset;	// 앞에 붙어있는 패킷만큼 건너뛰어야 FBuffData 배열이 시작됨
	uint16 NumBuff;

	bool IsValid() 
	{
		// 최소한 PacketHeader는 들어왔음이 보장, 패킷 크기가 구조체 크기보다 작다면 유효하지 않음
		uint32 FixedSize = sizeof(ServerPacket_Test);
		if (PacketSize < sizeof(ServerPacket_Test))
		{
			return false;
		}

		// 버프 리스트의 오프셋이 정상적인지 확인(오프셋 + 버프 리스트 길이가 패킷 전체 크기를 넘어가면 엉뚱한 오프셋)
		uint32 BuffsSize = NumBuff * sizeof(FBuffData);
		if (BuffsOffset + BuffsSize > PacketSize)
		{
			return false;
		}

		uint32 VictimsSize = 0;
		for (const FBuffData& Buff : GetBuffs())
		{
			if (!Buff.IsValid(reinterpret_cast<BYTE*>(this), PacketSize, VictimsSize))
			{
				return false;
			}
		}

		// 패킷 헤더에 기록된 전체 크기가 실제로 지금 계산해본 크기와 일치하는지 확인
		return FixedSize + BuffsSize + VictimsSize == PacketSize;
	}

	using FBuffDataArray = TPacketArray<FBuffData>;
	FBuffDataArray GetBuffs()
	{
		BYTE* Data = reinterpret_cast<BYTE*>(this) + BuffsOffset;
		return FBuffDataArray(reinterpret_cast<FBuffData*>(Data), NumBuff);
	}

	using FVictimArray = TPacketArray<uint64>;
	FVictimArray GetVictims(const FBuffData& Buff)
	{
		BYTE* Data = reinterpret_cast<BYTE*>(this) + Buff.VictimsOffset;
		return FVictimArray(reinterpret_cast<uint64*>(Data), Buff.NumVictims);
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
	for (auto& Buff : Buffs)
	{
		printf("\tBuff ID: %llu, Remain Time: %.2f\n", Buff.BuffId, Buff.RemainTime);

		auto Victims = Packet->GetVictims(Buff);
		printf("\tNum Victims: %hu, Victim ID: ", Victims.Num());

		for (uint64 Victim : Victims)
		{
			printf("%llu  ", Victim);
		}

		printf("\n");
	}
}
