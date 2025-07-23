#include "pch.h"
#include "IOCPEvent.h"

FSocketEvent::FSocketEvent(ESocketEventTypes InType)
	: Type(InType)
{
	Init();
}

void FSocketEvent::Init()
{
	Super::Internal = 0;
	Super::InternalHigh = 0;
	Super::Offset = 0;
	Super::OffsetHigh = 0;
	Super::Pointer = nullptr; // Pointer는 사용하지 않음
	Super::hEvent = nullptr; // hEvent는 사용하지 않음
}

FSocketConnect::FSocketConnect()
	: Super(ESocketEventTypes::Connect)
{
}

FSocketAccept::FSocketAccept()
	: Super(ESocketEventTypes::Accept)
	, Session(nullptr)
{
}

FSocketRecv::FSocketRecv()
	: Super(ESocketEventTypes::Recv)
{
}

FSocketSend::FSocketSend()
	: Super(ESocketEventTypes::Send)
{
}
