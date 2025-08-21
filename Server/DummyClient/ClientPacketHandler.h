#pragma once
#include "Protocol.pb.h"

using FPacketHandlerFunc = TFunction<bool(FPacketSessionRef&, BYTE*, int32)>;
extern FPacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_CHAT = 1004,
	PKT_S_CHAT = 1005,
};

// Custom Handlers
bool Handle_INVALID(FPacketSessionRef& InSession, BYTE* InBuffer, int32 InLength);
bool Handle_S_LOGIN(FPacketSessionRef& InSession, Protocol::S_LOGIN& Packet);
bool Handle_S_ENTER_GAME(FPacketSessionRef& InSession, Protocol::S_ENTER_GAME& Packet);
bool Handle_S_CHAT(FPacketSessionRef& InSession, Protocol::S_CHAT& Packet);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}
		GPacketHandler[PKT_S_LOGIN] = [](FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
			{
				return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, Session, Buffer, Length);
			};
		GPacketHandler[PKT_S_ENTER_GAME] = [](FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
			{
				return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, Session, Buffer, Length);
			};
		GPacketHandler[PKT_S_CHAT] = [](FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
			{
				return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, Session, Buffer, Length);
			};
	}

	static bool HandlePacket(FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
	{
		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
		return GPacketHandler[PacketHeader->Id](Session, Buffer, Length);
	}
	static FSendBufferRef MakeSendBuffer(Protocol::C_LOGIN& Packet) { return MakeSendBuffer(Packet, PKT_C_LOGIN); }
	static FSendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME& Packet) { return MakeSendBuffer(Packet, PKT_C_ENTER_GAME); }
	static FSendBufferRef MakeSendBuffer(Protocol::C_CHAT& Packet) { return MakeSendBuffer(Packet, PKT_C_CHAT); }

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

		FSendBufferRef SendBuffer = MakeShared<FSendBuffer>(PacketSize);
		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(SendBuffer->GetData());
		PacketHeader->Size = PacketSize;
		PacketHeader->Id = PacketId;
		check(Packet.SerializeToArray(&PacketHeader[1], DataSize));
		SendBuffer->Close(PacketSize);

		return SendBuffer;
	}
};