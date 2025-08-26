#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "S1.h"
#endif

using FPacketHandlerFunc = TFunction<bool(FPacketSessionRef&, BYTE*, int32)>;
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
		PacketHeader* Header = reinterpret_cast<PacketHeader*>(Buffer);
		return GPacketHandler[Header->Id](Session, Buffer, Length);
	}

{%- for pkt in parser.send_pkt %}
	static FSendBufferRef MakeSendBuffer(Protocol::{{pkt.name}}& Packet) { return MakeSendBuffer(Packet, PKT_{{pkt.name}}); }
{%- endfor %}

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc Function, FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
	{
		PacketType Packet;
		if (Packet.ParseFromArray(Buffer + sizeof(PacketHeader), Length - sizeof(PacketHeader)) == false)
			return false;

		return Function(Session, Packet);
	}

	template<typename T>
	static FSendBufferRef MakeSendBuffer(T& Packet, uint16 PacketId)
	{
		const uint16 DataSize = static_cast<uint16>(Packet.ByteSizeLong());
		const uint16 PacketSize = DataSize + sizeof(PacketHeader);

		FSendBufferRef SendBuffer = MakeShared<FSendBuffer>(PacketSize);
		PacketHeader* Header = reinterpret_cast<PacketHeader*>(SendBuffer->GetData());
		Header->Size = PacketSize;
		Header->Id = PacketId;
		check(Packet.SerializeToArray(&Header[1], DataSize));
		SendBuffer->Close(PacketSize);

		return SendBuffer;
	}
};
