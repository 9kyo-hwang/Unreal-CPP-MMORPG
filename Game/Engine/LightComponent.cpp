#include "pch.h"
#include "LightComponent.h"

#include "Resources.h"
#include "SceneComponent.h"

ULightComponent::ULightComponent()
	: Super(EComponentType::Light)
	, Info()
	, Index(0)
{
}

ULightComponent::~ULightComponent() = default;

void ULightComponent::FinalUpdate(float DeltaTime)
{
	Info.Position = GetTransform()->GetWorldPosition();
}

void ULightComponent::Render()
{
	assert(Index >= 0);

	GetTransform()->PushData();	// VolumeMesh에 대한 데이터 추가

	Material->SetParameter(0, Index);
	Material->PushGraphicsData();	// 셰이더에게 내 정보를 넘겨줌

	switch (StaticCast<ELightType>(Info.Type))
	{
	case ELightType::Point:
	case ELightType::Spot:
		float Scale = 2 * Info.Range;	// 기본값이 0.5이기 때문에 2배
		GetTransform()->SetLocalScale(FVector3(Scale, Scale, Scale));
		break;
	}

	VolumeMesh->Render();
}

void ULightComponent::SetType(ELightType NewType)
{
	Info.Type = StaticCast<int32>(NewType);

	switch (NewType)
	{
	case ELightType::Directional:
		VolumeMesh = Resources::Get()->Get<UMesh>(L"Rectangle");
		Material = Resources::Get()->Get<FMaterial>(L"DirectionalLight");
		break;
	case ELightType::Point:
		VolumeMesh = Resources::Get()->Get<UMesh>(L"Sphere");
		Material = Resources::Get()->Get<FMaterial>(L"PointLight");
		break;
	case ELightType::Spot:	// 엄밀히 따지면 원뿔 모양 콘을 제작해야 하나, 일단은 Point 형태를 재사용
		VolumeMesh = Resources::Get()->Get<UMesh>(L"Sphere");
		Material = Resources::Get()->Get<FMaterial>(L"PointLight");
		break;
	}
}
