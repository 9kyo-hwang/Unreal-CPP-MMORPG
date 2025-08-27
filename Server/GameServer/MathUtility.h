#pragma once
#include <random>

struct FMath
{
	static FORCEINLINE float FRand()
	{
		return (rand() & RAND_MAX) / static_cast<float>(RAND_MAX);
	}

	[[nodiscard]] static FORCEINLINE int32 RandHelper(int32 A)
	{
		return A > 0 ? min<int32>(trunc(FRand() * static_cast<float>(A)), A - 1) : 0;
	}

	[[nodiscard]] static FORCEINLINE int64 RandHelper64(int64 A)
	{
		return A > 0 ? min<int64>(trunc(FRand() * static_cast<float>(A)), A - 1) : 0;
	}

	[[nodiscard]] static FORCEINLINE int32 RandRange(int32 Min, int32 Max)
	{
		const int32 Range = (Max - Min) + 1;
		return Min + RandHelper(Range);
	}

	[[nodiscard]] static FORCEINLINE int64 RandRange(int64 Min, int64 Max)
	{
		const int64 Range = (Max - Min) + 1;
		return Min + RandHelper64(Range);
	}

	[[nodiscard]] static FORCEINLINE float RandRange(float InMin, float InMax)
	{
		return FRandRange(InMin, InMax);
	}

	[[nodiscard]] static FORCEINLINE double RandRange(double InMin, double InMax)
	{
		return FRandRange(InMin, InMax);
	}

	[[nodiscard]] static FORCEINLINE float FRandRange(float InMin, float InMax)
	{
		return InMin + (InMax - InMin) * FRand();
	}

	[[nodiscard]] static FORCEINLINE double FRandRange(double InMin, double InMax)
	{
		return InMin + (InMax - InMin) * FRand();
	}

	[[nodiscard]] static FORCEINLINE bool RandBool()
	{
		return RandRange(0, 1) == 1;
	}

	// 강의 코드
	template<typename T>
	[[nodiscard]] static FORCEINLINE T GetRandom(T Min, T Max)
	{
		std::random_device Device;
		std::mt19937 Engine(Device);

		if constexpr (std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> Distribution(Min, Max);
			return Distribution(Engine);
		}
		else
		{
			std::uniform_real_distribution<T> Distribution(Min, Max);
			return Distribution(Engine);
		}
	}

private:
	struct FRandomStream
	{
		FRandomStream()
		{
			srand(static_cast<uint32>(time(nullptr)));
		}
	};

	static inline FRandomStream RandomStream;
};
