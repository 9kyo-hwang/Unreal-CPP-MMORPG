#include "pch.h"
#include "Debug.h"

FDebug::FDebug()
	: StdOut(nullptr)
	, StdErr(nullptr)
{
}

FDebug::~FDebug()
{
}

void FDebug::Log(EColor Color, const TCHAR* Message, ...) const
{
	if (Message == nullptr)
	{
		return;
	}

	SetColor(true, Color);

	va_list Args;
	va_start(Args, Message);
	::vwprintf(Message, Args);
	va_end(Args);

	fflush(stdout);

	SetColor(true, EColor::White);
}

void FDebug::LogError(EColor Color, const TCHAR* Message, ...) const
{
	if (Message == nullptr)
	{
		return;
	}

	SetColor(false, Color);

	TCHAR Buffer[BufferSize];
	va_list Args;
	va_start(Args, Message);
	::vswprintf_s(Buffer, BufferSize, Message, Args);
	va_end(Args);

	::fwprintf_s(stderr, Buffer);
	fflush(stderr);

	SetColor(false, EColor::White);
}

void FDebug::SetColor(bool bIsStdOut, EColor Color) const
{
	static uint16 ColorTable[]
	{
		0,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_RED | FOREGROUND_INTENSITY,
		FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
	};

	::SetConsoleTextAttribute(bIsStdOut ? StdOut : StdErr, ColorTable[static_cast<int32>(Color)]);
}
