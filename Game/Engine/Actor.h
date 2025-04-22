#pragma once
#include "ActorComponent.h"
#include "Object.h"
#include "TimeManager.h"

class UParticleSystemComponent;
class ULightComponent;
class UCameraComponent;
class UMeshComponent;
class MonoBehaviour;
class USceneComponent;

class AActor : public UObject, public TSharedFromThis<AActor>
{
	using Super = UObject;

public:
	AActor();
	~AActor() override;

	// 들고 있는 Component들의 Event Function들을 실행시킬 것임
	void Initialize();
	void BeginPlay();
	void Tick(float DeltaTime = TimeManager::Get()->GetDeltaTime());
	void LateUpdate(float DeltaTime = TimeManager::Get()->GetDeltaTime());
	void FinalUpdate(float DeltaTime = TimeManager::Get()->GetDeltaTime());

	TSharedPtr<UActorComponent> GetComponent(EComponentType Type) const;
	void AddComponent(TSharedPtr<UActorComponent> Component);

public:
	TSharedPtr<USceneComponent> GetSceneComponent() const;
	TSharedPtr<UMeshComponent> GetMeshComponent() const;
	TSharedPtr<UCameraComponent> GetCameraComponent() const;
	TSharedPtr<ULightComponent> GetLightComponent() const;
	TSharedPtr<UParticleSystemComponent> GetParticleSystemComponent() const;

	bool GetCheckFrustum() const { return bCheckFrustum; }
	void SetCheckFrustum(bool Value) { bCheckFrustum = Value; }

	uint8 GetLayer() const { return Layer; }
	void SetLayer(uint8 InLayer) { Layer = InLayer; }

	bool IsStaticShadow() const { return bStaticShadow; }
	void SetIsStaticShadow(bool Value) { bStaticShadow = Value; }

private:
	array<TSharedPtr<UActorComponent>, ComponentCount> Components;	// 컴포넌트는 유일하게 1개씩만
	vector<TSharedPtr<MonoBehaviour>> Scripts;	// 스크립트는 동적으로 여러 개

	bool bCheckFrustum;
	uint8 Layer;
	bool bStaticShadow;
};

