#include "pch.h"
#include "Transform.h"

Transform::Transform()
	: Super(EComponentType::Transform)
{
}

Transform::~Transform()
{
}

// TODO: CONSTANT_BUFFER(EConstantBufferType::Transform)->Add(&Transform, sizeof(Transform));	// FConstantBuffer::Add()에서 SetShaderResourceView()를 자동적으로 수행해주고 있음.
