#include "pch.h"
#include "Light.h"

#include "Transform.h"

Light::Light()
	: Super(EComponentType::Light)
	, Info()
{
}

Light::~Light()
{
}

void Light::FinalUpdate()
{
	Info.Position = GetTransform()->GetWorldPosition();
}
