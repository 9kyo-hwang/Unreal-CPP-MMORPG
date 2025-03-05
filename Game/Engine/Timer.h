#pragma once

class FTimer
{
public:
	FTimer();
	~FTimer();

	void Initialize();
	void Update();

	uint32 GetFPS() const { return FPS; }
	float GetDeltaTime() const { return DeltaTime; }

private:
	uint64 Frequency;
	uint64 PrevCount;
	float DeltaTime;

private:
	uint32 FrameCount;
	float FrameTime;
	uint32 FPS;
};

