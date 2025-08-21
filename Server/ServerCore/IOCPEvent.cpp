#include "pch.h"
#include "IocpEvent.h"

/*--------------
	FSocketIOEvent
---------------*/

FSocketIOEvent::FSocketIOEvent(EEventType type) : EventType(type)
{
	Init();
}

void FSocketIOEvent::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
