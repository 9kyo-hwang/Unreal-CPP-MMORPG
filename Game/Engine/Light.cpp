#include "pch.h"
#include "Light.h"

#include "Resources.h"
#include "Transform.h"

Light::Light()
	: Super(EComponentType::Light)
	, Info()
	, Index(0)
{
}

Light::~Light()
{
}

void Light::FinalUpdate()
{
	Info.Position = GetTransform()->GetWorldPosition();
}

void Light::Render()
{
	assert(Index >= 0);

	GetTransform()->PushData();	// VolumeMesh에 대한 데이터 추가

	Material->SetParameter(0, Index);
	Material->PushGraphicsData();	// 셰이더에게 내 정보를 넘겨줌

	switch (static_cast<ELightType>(Info.Type))
	{
	case ELightType::Directional:
		break;
	case ELightType::Point:
	case ELightType::Spot:
		float Scale = 2 * Info.Range;	// 기본값이 0.5이기 때문에 2배
		GetTransform()->SetLocalScale(FVector3(Scale, Scale, Scale));
		break;
	}

	VolumeMesh->Render();
}

void Light::SetType(ELightType NewType)
{
	Info.Type = static_cast<int32>(NewType);

	switch (NewType)
	{
	case ELightType::Directional:
		VolumeMesh = Resources::Get()->Get<FMesh>(L"Rectangle");
		Material = Resources::Get()->Get<FMaterial>(L"DirectionalLight");
		break;
	case ELightType::Point:
		VolumeMesh = Resources::Get()->Get<FMesh>(L"Sphere");
		Material = Resources::Get()->Get<FMaterial>(L"PointLight");
		break;
	case ELightType::Spot:	// 엄밀히 따지면 원뿔 모양 콘을 제작해야 하나, 일단은 Point 형태를 재사용
		VolumeMesh = Resources::Get()->Get<FMesh>(L"Sphere");
		Material = Resources::Get()->Get<FMaterial>(L"PointLight");
		break;
	}
}
