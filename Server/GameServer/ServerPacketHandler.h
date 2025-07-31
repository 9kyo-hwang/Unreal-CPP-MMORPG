#pragma once

enum EPacketId
{
	Test = 1,
};

struct FBuffData
{
	uint64 Id;
	float RemainTime;
};

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* Buffer, int32 Length);

	static shared_ptr<FSendBuffer> CreatePacket_Test(uint64 Id, uint32 Hp, uint16 Attack, TArray<FBuffData> Buffs, FString Name);
};

