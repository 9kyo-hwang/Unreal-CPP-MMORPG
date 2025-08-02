#pragma once
#include "Protocol.pb.h"
#include "Session.h"

using FIncomingPacketSignature = function<bool(shared_ptr<FPacketSession>&, BYTE*, int32)>;
extern FIncomingPacketSignature GPacketHandlers[UINT16_MAX];

enum EPacketId : uint16
{
{%- for packet in parser.packets %}
	{{packet.name}} = {{packet.packet_id}},
{%- endfor %}
};

bool Handle_INVALID(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length);

{%- for packet in parser.recv_packets %}
bool Handle_{{packet.name}}(shared_ptr<FPacketSession>& Session, Protocol::{{packet.name}}& Packet);
{%- endfor %}

class {{output}}
{
public:
	static void Initialize()
	{
		for (int32 i = 0; i < UINT16_MAX; ++i)
		{
			GPacketHandlers[i] = Handle_INVALID;
		}

{%- for packet in parser.recv_packets %}
		GPacketHandlers[EPacketId::{{packet.name}}] = [](shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
			{
				return Incoming_Internal<Protocol::{{packet.name}}>(Handle_{{packet.name}}, Session, Buffer, Length);
			};
{%- endfor %}
	}

	static bool Incoming(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
	{
		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
		return GPacketHandlers[PacketHeader->Id](Session, Buffer, Length);
	}

{%- for packet in parser.send_packets %}
	static shared_ptr<FSendBuffer> CreateSendBuffer(Protocol::{{packet.name}}& Packet)
	{
		return CreateSendBuffer_Internal(Packet, EPacketId::{{packet.name}});
	}
{%- endfor %}

private:
	template<typename InPacketType, typename InHandlerType>
	static bool Incoming_Internal(InHandlerType Handler, shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
	{
		InPacketType Packet;
		if (!Packet.ParseFromArray(Buffer + sizeof(FPacketHeader), Length - sizeof(FPacketHeader)))
		{
			return false;
		}

		return Handler(Session, Packet);
	}

	template<typename PacketType>
	static shared_ptr<FSendBuffer> CreateSendBuffer_Internal(PacketType& Packet, uint16 PacketId)
	{
		const uint16 DataSize = static_cast<uint16>(Packet.ByteSizeLong());
		const uint16 PacketSize = DataSize + sizeof(FPacketHeader);

		shared_ptr<FSendBuffer> SendBuffer = GSendBufferPool->Open(PacketSize);

		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(SendBuffer->GetData());
		PacketHeader->Size = PacketSize;
		PacketHeader->Id = PacketId;

		check(Packet.SerializeToArray(&PacketHeader[1], DataSize));

		SendBuffer->Close(PacketSize);
		return SendBuffer;
	}
};