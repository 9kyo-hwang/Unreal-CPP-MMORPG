#pragma once
#include "Protocol.pb.h"
#include "Session.h"

using FIncomingPacketSignature = function<bool(shared_ptr<FPacketSession>&, BYTE*, int32)>;
extern FIncomingPacketSignature GPacketHandlers[UINT16_MAX];

enum EPacketId : uint16
{
	C_LOGIN = 1000,
	S_LOGIN = 1001,
	C_ENTER = 1002,
	S_ENTER = 1003,
	C_CHAT = 1004,
	S_CHAT = 1005,
};

bool Handle_INVALID(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length);
bool Handle_S_LOGIN(shared_ptr<FPacketSession>& Session, Protocol::S_LOGIN& Packet);
bool Handle_S_ENTER(shared_ptr<FPacketSession>& Session, Protocol::S_ENTER& Packet);
bool Handle_S_CHAT(shared_ptr<FPacketSession>& Session, Protocol::S_CHAT& Packet);

class ServerPacketHandler
{
public:
	static void Initialize()
	{
		for (int32 i = 0; i < UINT16_MAX; ++i)
		{
			GPacketHandlers[i] = Handle_INVALID;
		}
		GPacketHandlers[EPacketId::S_LOGIN] = [](shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
			{
				return Incoming_Internal<Protocol::S_LOGIN>(Handle_S_LOGIN, Session, Buffer, Length);
			};
		GPacketHandlers[EPacketId::S_ENTER] = [](shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
			{
				return Incoming_Internal<Protocol::S_ENTER>(Handle_S_ENTER, Session, Buffer, Length);
			};
		GPacketHandlers[EPacketId::S_CHAT] = [](shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
			{
				return Incoming_Internal<Protocol::S_CHAT>(Handle_S_CHAT, Session, Buffer, Length);
			};
	}

	static bool Incoming(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
	{
		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
		return GPacketHandlers[PacketHeader->Id](Session, Buffer, Length);
	}
	static shared_ptr<FSendBuffer> CreateSendBuffer(Protocol::C_LOGIN& Packet)
	{
		return CreateSendBuffer_Internal(Packet, EPacketId::C_LOGIN);
	}
	static shared_ptr<FSendBuffer> CreateSendBuffer(Protocol::C_ENTER& Packet)
	{
		return CreateSendBuffer_Internal(Packet, EPacketId::C_ENTER);
	}
	static shared_ptr<FSendBuffer> CreateSendBuffer(Protocol::C_CHAT& Packet)
	{
		return CreateSendBuffer_Internal(Packet, EPacketId::C_CHAT);
	}

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