#pragma once
#include "Component.h"

struct FTransformMatrix
{
	FVector4 Offset;  // Shader에서 float4 offset 1개를 들고 있기 때문에 타입을 맞춰줌
};

class Transform : public Component
{
	using Super = Component;

public:
	Transform();
	~Transform() override;

	// TODO: Parent/Child 관계 설정

private:
	// TODO: World Position 세팅
};
