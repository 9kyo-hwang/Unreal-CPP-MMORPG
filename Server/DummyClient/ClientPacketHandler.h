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