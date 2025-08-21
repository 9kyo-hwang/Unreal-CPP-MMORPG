#pragma once
#include "Protocol.pb.h"

using FPacketHandlerFunc = std::function<bool(FPacketSessionRef&, BYTE*, int32)>;
extern FPacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

// Custom Handlers
bool Handle_INVALID(FPacketSessionRef& InSession, BYTE* InBuffer, int32 InLength);

{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(FPacketSessionRef& InSession, Protocol::{{pkt.name}}& Packet);
{%- endfor %}

class {{output}}
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}

{%- for pkt in parser.recv_pkt %}
		GPacketHandler[PKT_{{pkt.name}}] = [](FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
			{
				return HandlePacket<Protocol::{{pkt.name}}>(Handle_{{pkt.name}}, Session, Buffer, Length);
			};
{%- endfor %}
	}

	static bool HandlePacket(FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
	{
		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
		return GPacketHandler[PacketHeader->Id](Session, Buffer, Length);
	}

{%- for pkt in parser.send_pkt %}
	static FSendBufferRef MakeSendBuffer(Protocol::{{pkt.name}}& Packet) { return MakeSendBuffer(Packet, PKT_{{pkt.name}}); }
{%- endfor %}

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc Function, FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
	{
		PacketType Packet;
		if (Packet.ParseFromArray(Buffer + sizeof(FPacketHeader), Length - sizeof(FPacketHeader)) == false)
			return false;

		return Function(Session, Packet);
	}

	template<typename T>
	static FSendBufferRef MakeSendBuffer(T& Packet, uint16 PacketId)
	{
		const uint16 DataSize = static_cast<uint16>(Packet.ByteSizeLong());
		const uint16 PacketSize = DataSize + sizeof(FPacketHeader);

		FSendBufferRef SendBuffer = make_shared<FSendBuffer>(PacketSize);
		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(SendBuffer->GetData());
		PacketHeader->Size = PacketSize;
		PacketHeader->Id = PacketId;
		check(Packet.SerializeToArray(&PacketHeader[1], DataSize));
		SendBuffer->Close(PacketSize);

		return SendBuffer;
	}
};
