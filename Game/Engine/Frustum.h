#pragma once

enum class EPlaneType : uint8
{
	Front,
	Back,
	Up,
	Down,
	Left,
	Right,

	END
};

class Frustum
{
public:
	void FinalUpdate();
	bool ContainsSphere(const FVector3& Position, float Radius);

private:
	array<FVector4, static_cast<size_t>(EPlaneType::END)> Planes;
};
