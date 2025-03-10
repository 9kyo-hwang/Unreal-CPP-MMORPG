#include "pch.h"
#include "TimeManager.h"

void TimeManager::Initialize()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&Frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&PrevCount));
}

void TimeManager::Update()
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
