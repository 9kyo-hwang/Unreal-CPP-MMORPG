#pragma once

enum class EColor
{
	Black,
	White,
	Red,
	Green,
	Blue,
	Yellow
};

class FDebug
{
	static constexpr int16 BufferSize = 0x1000;

public:
	FDebug();
	~FDebug();

	void Log(EColor Color, const TCHAR* Message, ...) const;
	void LogError(EColor Color, const TCHAR* Message, ...) const;

protected:
	void SetColor(bool bIsStdOut, EColor Color) const;

private:
	HANDLE StdOut;
	HANDLE StdErr;
};

