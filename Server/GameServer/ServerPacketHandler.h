#pragma once
#include "Protocol.pb.h"
#include "Session.h"

using FIncomingPacketSignature = function<bool(shared_ptr<FPacketSession>&, BYTE*, int32)>;
extern FIncomingPacketSignature GPacketHandlers[UINT16_MAX];

// TODO: 자동화(.proto 파일을 보고 자동으로 Id Enum 값을 생성해주는 작업을 수행할 예정)
enum EPacketId : uint16
{
	Test = 1,
	Login = 2,
};

// TODO: 자동화
bool Handle_INVALID(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length);
bool Handle_S_TEST(shared_ptr<FPacketSession>& Session, Protocol::S_TEST& Packet);

class ServerPacketHandler
{
public:
	// TODO: 자동화
	static void Initialize()
	{
		for (int32 i = 0; i < UINT16_MAX; ++i)
		{
			GPacketHandlers[i] = Handle_INVALID;
		}

		GPacketHandlers[EPacketId::Test] = [](shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
			{
				return Incoming_Internal<Protocol::S_TEST>(Handle_S_TEST, Session, Buffer, Length);
			};
	}

	static bool Incoming(shared_ptr<FPacketSession>& Session, BYTE* Buffer, int32 Length)
	{
		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer);
		return GPacketHandlers[PacketHeader->Id](Session, Buffer, Length);
	}

	// TODO: 자동화
	static shared_ptr<FSendBuffer> CreateSendBuffer(Protocol::S_TEST& Packet)
	{
		return CreateSendBuffer_Internal<Protocol::S_TEST>(Packet, EPacketId::Test);
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

		// 패킷 사이즈를 미리 알 수 있으므로 SendBuffer를 할당받을 때도 그 크기 정보를 이용
		shared_ptr<FSendBuffer> SendBuffer = GSendBufferPool->Open(PacketSize);

		// 단순히 헤더 정보만 채우면 돼서 Writer를 쓰지 않고 형변환을 이용해 작성
		FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(SendBuffer->GetData());
		PacketHeader->Size = PacketSize;
		PacketHeader->Id = PacketId;

		// 패킷 헤더의 끝부분부터 데이터를 담는 공간, 헤더가 정확히 4바이트라 1번 인덱스의 주소를 넘겨주면 됨
		check(Packet.SerializeToArray(&PacketHeader[1], DataSize));

		SendBuffer->Close(PacketSize);
		return SendBuffer;
	}
};