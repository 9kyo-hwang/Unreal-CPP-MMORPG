#include "pch.h"
#include "TimeManager.h"

void TimeManager::Initialize()
{
	::QueryPerformanceFrequency(ReinterpretCast<LARGE_INTEGER>(&Frequency));
	::QueryPerformanceCounter(ReinterpretCast<LARGE_INTEGER>(&PrevCount));
}

void TimeManager::Update()
{
	uint64 CurrentCount;
	::QueryPerformanceCounter(ReinterpretCast<LARGE_INTEGER>(&CurrentCount));

	DeltaTime = ( CurrentCount - PrevCount ) / StaticCast<float>(Frequency);
	PrevCount = CurrentCount;

	FrameCount++;
	FrameTime += DeltaTime;

	if (FrameTime > 1.f)
	{
		FPS = StaticCast<uint32>(FrameCount / FrameTime);
		FrameTime = 0.f;
		FrameCount = 0;
	}
}
