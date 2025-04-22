#pragma once
#include "ActorComponent.h"

class USceneComponent : public UActorComponent
{
	using Super = UActorComponent;

public:
	USceneComponent();
	~USceneComponent() override;

	// TODO: Parent/Child 관계 설정
	void FinalUpdate(float DeltaTime) override;
	void PushData();

public:
	const FVector3& GetLocalPosition() const { return LocalPosition; }
	const FVector3& GetLocalRotation() const { return LocalRotation; }
	const FVector3& GetLocalScale() const { return LocalScale; }

	// TEMP
	float GetFrustumBound() const { return max(max(LocalScale.x, LocalScale.y), LocalScale.z); }

	// TODO: const& 반환이 아닌 복사 반환으로?
	FMatrix GetLocalToWorldMatrix() const { return WorldMatrix; }
	FVector3 GetWorldPosition() const { return WorldMatrix.Translation(); }

	// 아래 3종류 벡터는 월드 좌표계를 기준으로 계산됨
	FVector3 GetRight() const { return WorldMatrix.Right(); }
	FVector3 GetUp() const { return WorldMatrix.Up(); }
	FVector3 GetForward() const { return WorldMatrix.Backward(); }

	void SetLocalPosition(const FVector3& Position) { LocalPosition = Position; }
	void SetLocalRotation(const FVector3& Rotation) { LocalRotation = Rotation; }
	void SetLocalScale(const FVector3& Scale) { LocalScale = Scale; }

	void LookAt(const FVector3& Direction);

	static bool CloseEnough(const float& Lhs, const float& Rhs, const float& Epsilon = std::numeric_limits<float>::epsilon());
	static FVector3 DecomposeRotationMatrix(const FMatrix& Rotation);

	TWeakPtr<USceneComponent> GetOwner() { return Owner; }
	void SetOwner(TSharedPtr<USceneComponent> NewOwner) { Owner = NewOwner; }

private:
	// Parent 기준 위치/회전/크기
	FVector3 LocalPosition;
	FVector3 LocalRotation;
	FVector3 LocalScale;

	FMatrix LocalMatrix;	// 부모 기준 좌표계를 저장할 임시 변수
	FMatrix WorldMatrix;	// Local 좌표계를 World 좌표계로 변환하기 위한 행렬(FinalUpdate에서 수행)
							// WorldMatrix를 이용해 월드 좌표계 기준의 내 좌표는 GetWorldPosition()을 통해 획득

	TWeakPtr<USceneComponent> Owner;
};
