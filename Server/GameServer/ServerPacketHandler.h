#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "S1.h"
#endif

using FPacketHandlerFunc = TFunction<bool(FPacketSessionRef&, BYTE*, int32)>;
extern FPacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_LEAVE_GAME = 1004,
	PKT_S_LEAVE_GAME = 1005,
	PKT_S_SPAWN = 1006,
	PKT_S_DESPAWN = 1007,
	PKT_C_MOVE = 1008,
	PKT_S_MOVE = 1009,
	PKT_C_CHAT = 1010,
	PKT_S_CHAT = 1011,
};

// Custom Handlers
bool Handle_INVALID(FPacketSessionRef& InSession, BYTE* InBuffer, int32 InLength);
bool Handle_C_LOGIN(FPacketSessionRef& InSession, Protocol::C_LOGIN& Packet);
bool Handle_C_ENTER_GAME(FPacketSessionRef& InSession, Protocol::C_ENTER_GAME& Packet);
bool Handle_C_LEAVE_GAME(FPacketSessionRef& InSession, Protocol::C_LEAVE_GAME& Packet);
bool Handle_C_MOVE(FPacketSessionRef& InSession, Protocol::C_MOVE& Packet);
bool Handle_C_CHAT(FPacketSessionRef& InSession, Protocol::C_CHAT& Packet);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}
		GPacketHandler[PKT_C_LOGIN] = [](FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
			{
				return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, Session, Buffer, Length);
			};
		GPacketHandler[PKT_C_ENTER_GAME] = [](FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
			{
				return HandlePacket<Protocol::C_ENTER_GAME>(Handle_C_ENTER_GAME, Session, Buffer, Length);
			};
		GPacketHandler[PKT_C_LEAVE_GAME] = [](FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
			{
				return HandlePacket<Protocol::C_LEAVE_GAME>(Handle_C_LEAVE_GAME, Session, Buffer, Length);
			};
		GPacketHandler[PKT_C_MOVE] = [](FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
			{
				return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, Session, Buffer, Length);
			};
		GPacketHandler[PKT_C_CHAT] = [](FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
			{
				return HandlePacket<Protocol::C_CHAT>(Handle_C_CHAT, Session, Buffer, Length);
			};
	}

	static bool HandlePacket(FPacketSessionRef& Session, BYTE* Buffer, int32 Length)
	{
		PacketHeader* Header = reinterpret_cast<PacketHeader*>(Buffer);
		return GPacketHandler[Header->Id](Session, Buffer, Length);
	}
	static FSendBufferRef MakeSendBuffer(Protocol::S_LOGIN& Packet) { return MakeSendBuffer(Packet, PKT_S_LOGIN); }
	static FSendBufferRef MakeSendBuffer(Protocol::S_ENTER_GAME& Packet) { return MakeSendBuffer(Packet, PKT_S_ENTER_GAME); }
	static FSendBufferRef MakeSendBuffer(Protocol::S_LEAVE_GAME& Packet) { return MakeSendBuffer(Packet, PKT_S_LEAVE_GAME); }
	static FSendBufferRef MakeSendBuffer(Protocol::S_SPAWN& Packet) { return MakeSendBuffer(Packet, PKT_S_SPAWN); }
	static FSendBufferRef MakeSendBuffer(Protocol::S_DESPAWN& Packet) { return MakeSendBuffer(Packet, PKT_S_DESPAWN); }
	static FSendBufferRef MakeSendBuffer(Protocol::S_MOVE& Packet) { return MakeSendBuffer(Packet, PKT_S_MOVE); }
	static FSendBufferRef MakeSendBuffer(Protocol::S_CHAT& Packet) { return MakeSendBuffer(Packet, PKT_S_CHAT); }

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