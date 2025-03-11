#pragma once
#include "Component.h"

enum class ECameraProjectionType : uint8
{
	Perspective,	// 원근
	Orthographic,	// 직교
};

class Camera : public Component
{
	using Super = Component;

public:
	Camera();
	~Camera() override;

	void FinalUpdate() override;
	void Render();

private:
	ECameraProjectionType Type;

	float Near;
	float Far;
	float Fov;
	float Scale;

	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;

public:
	// TEMP
	static FMatrix StaticViewMatrix;
	static FMatrix StaticProjectionMatrix;
};

