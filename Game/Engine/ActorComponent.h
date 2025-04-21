#pragma once
#include "Object.h"

enum class EComponentType : uint8
{
	Scene,
	Mesh,
	Camera,
	Light,
	ParticleSystem,
	Movement,
	// ...
	MonoBehaviour,	// 항상 마지막에 위치
	END
};

constexpr uint8 ComponentCount = ConstexprCast<uint8>(EComponentType::END) - 1;

class AActor;
class USceneComponent;

class UActorComponent : public UObject	// MonoBehaviour 스크립트가 툴에서 Load/Save 가능하기 때문에 상속
{
	using Super = UObject;

public:
	UActorComponent(EComponentType InType);
	~UActorComponent() override;

	virtual void InitializeComponent() {}
	virtual void BeginPlay() {}
	virtual void TickComponent(float DeltaTime) {}
	virtual void LateUpdate(float DeltaTime) {}
	virtual void FinalUpdate(float DeltaTime) {}	// 행렬 연산 등 엔진 단에서 활용할 최종 업데이트

	EComponentType GetComponentType() const { return ComponentType; }
	TSharedPtr<AActor> GetOwner() const;
	TSharedPtr<USceneComponent> GetTransform() const;

private:
	friend class AActor;
	void SetOwner(const TSharedPtr<AActor>& InOwner) { Owner = InOwner; }

protected:
	EComponentType ComponentType;
	TWeakPtr<AActor> Owner;	// Actor에서도 ActorComponent를 SharedPtr로 참조할 예정. 순환 참조를 막기 위해
};

