#pragma once
#include "Protocol.pb.h"

using FPacketHandlerFunc = TFunction<bool(FPacketSessionRef&, BYTE*, int32)>;
extern FPacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_S_TEST = 1000,
};

// Custom Handlers
bool Handle_INVALID(FPacketSessionRef& session, BYTE* buffer, int32 len);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
	}

	static bool HandlePacket(FPacketSessionRef& session, BYTE* buffer, int32 len)
	{
		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);
		return GPacketHandler[header->Id](session, buffer, len);
	}
	static FSendBufferRef MakeSendBuffer(Protocol::S_TEST& pkt) { return FPacketSession::MakeSendBuffer(pkt, PKT_S_TEST); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, FPacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(FPacketHeader), len - sizeof(FPacketHeader)) == false)
			return false;

		return func(session, pkt);
	}
};