#include "pch.h"
#include "IOCPEvent.h"

FOverlapped::FOverlapped(EIoEvent InEventType)
	: EventType(InEventType)
{
	Init();
}

void FOverlapped::Init()
{
	Super::Internal = 0;
	Super::InternalHigh = 0;
	Super::Offset = 0;
	Super::OffsetHigh = 0;
	Super::Pointer = nullptr; // Pointer는 사용하지 않음
	Super::hEvent = nullptr; // hEvent는 사용하지 않음
}

FOverlapped_Connect::FOverlapped_Connect()
	: Super(EIoEvent::Connect)
{
}

FOverlapped_Accept::FOverlapped_Accept()
	: Super(EIoEvent::Accept)
	, Session(nullptr)
{
}

FSession* FOverlapped_Accept::GetSession()
{
	return Session;
}

void FOverlapped_Accept::SetSession(FSession* InSession)
{
	Session = InSession;
}

FOverlapped_Recv::FOverlapped_Recv()
	: Super(EIoEvent::Recv)
{
}

FOverlapped_Send::FOverlapped_Send()
	: Super(EIoEvent::Send)
{
}
