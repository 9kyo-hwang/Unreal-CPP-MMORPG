#include "pch.h"
#include "LightComponent.h"

#include "Resources.h"
#include "SceneComponent.h"
#include "CameraComponent.h"
#include "SceneManager.h"

ULightComponent::ULightComponent()
	: Super(EComponentType::Light)
	, Info()
	, Index(0)
{
	ShadowCameraActor = MakeShared<AActor>();
	ShadowCameraActor->AddComponent(MakeShared<USceneComponent>());
	ShadowCameraActor->AddComponent(MakeShared<UCameraComponent>());

	uint8 UILayer = SceneManager::Get()->NameToLayer(L"UI");
	ShadowCameraActor->GetCameraComponent()->EnableLayerCulling(UILayer, true);
}

ULightComponent::~ULightComponent() = default;

void ULightComponent::FinalUpdate(float DeltaTime)
{
	Info.Position = GetTransform()->GetWorldPosition();

	ShadowCameraActor->GetSceneComponent()->SetLocalPosition(GetTransform()->GetLocalPosition());
	ShadowCameraActor->GetSceneComponent()->SetLocalRotation(GetTransform()->GetLocalRotation());
	ShadowCameraActor->GetSceneComponent()->SetLocalScale(GetTransform()->GetLocalScale());

	ShadowCameraActor->FinalUpdate();
}

void ULightComponent::Render()
{
	assert(Index >= 0);

	GetTransform()->PushData();	// VolumeMesh에 대한 데이터 추가

	if (StaticCast<ELightType>(Info.Type) == ELightType::Directional)
	{
		TSharedPtr<FTexture> ShadowTexture = Resources::Get()->Get<FTexture>(L"ShadowTarget");
		Material->SetTexture(2, ShadowTexture);

		TSharedPtr<UCameraComponent> CameraComponent = ShadowCameraActor->GetCameraComponent();
		FMatrix ViewProjectionMatrix = CameraComponent->GetViewMatrix() * CameraComponent->GetProjectionMatrix();
		Material->SetParameter(0, ViewProjectionMatrix);
	}
	else
	{
		float Scale = 2 * Info.Range;	// 기본값이 0.5이기 때문에 2배
		GetTransform()->SetLocalScale(FVector3(Scale, Scale, Scale));
	}

	Material->SetParameter(0, Index);
	Material->PushGraphicsData();	// 셰이더에게 내 정보를 넘겨줌

	VolumeMesh->Render();
}

void ULightComponent::RenderShadow()
{
	ShadowCameraActor->GetCameraComponent()->SortShadowActors();
	ShadowCameraActor->GetCameraComponent()->RenderShadow();
}

void ULightComponent::SetDirection(FVector3 NewDirection)
{
	NewDirection.Normalize();
	Info.Direction = NewDirection;
	GetTransform()->LookAt(NewDirection);	// 실제 회전값도 변경된 방향에 맞게 반영
}

void ULightComponent::SetType(ELightType NewType)
{
	Info.Type = StaticCast<int32>(NewType);

	switch (NewType)
	{
	case ELightType::Directional:
		VolumeMesh = Resources::Get()->Get<UMesh>(L"Rectangle");
		Material = Resources::Get()->Get<FMaterial>(L"DirectionalLight");

		ShadowCameraActor->GetCameraComponent()->SetScale(1.f);
		ShadowCameraActor->GetCameraComponent()->SetFar(10000.f);
		ShadowCameraActor->GetCameraComponent()->SetWidth(4096);
		ShadowCameraActor->GetCameraComponent()->SetHeight(4096);
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
