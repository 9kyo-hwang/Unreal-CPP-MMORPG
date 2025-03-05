#include "pch.h"
#include "Timer.h"

FTimer::FTimer()
	: Frequency(0)
	, PrevCount(0)
	, DeltaTime(0.f)
	, FrameCount(0)
	, FrameTime(0.f)
	, FPS(0)
{
}

FTimer::~FTimer()
{
}

void FTimer::Initialize()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&Frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&PrevCount));
}

void FTimer::Update()
{
	uint64 CurrentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&CurrentCount));

	DeltaTime = ( CurrentCount - PrevCount ) / static_cast<float>(Frequency);
	PrevCount = CurrentCount;

	FrameCount++;
	FrameTime += DeltaTime;

	if (FrameTime > 1.f)
	{
		FPS = static_cast<uint32>(FrameCount / FrameTime);
		FrameTime = 0.f;
		FrameCount = 0;
	}
}
