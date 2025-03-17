#pragma once
#include "Component.h"

class Transform : public Component
{
	using Super = Component;

public:
	Transform();
	~Transform() override;

	// TODO: Parent/Child 관계 설정
	void FinalUpdate() override;
	void PushData();

public:
	const FVector3& GetLocalPosition() const { return LocalPosition; }
	const FVector3& GetLocalRotation() const { return LocalRotation; }
	const FVector3& GetLocalScale() const { return LocalScale; }

	// TODO: const& 반환이 아닌 복사 반환으로?
	const FMatrix& GetLocalToWorldMatrix() const { return WorldMatrix; }
	const FVector3& GetWorldPosition() const { return WorldMatrix.Translation(); }

	// 아래 3종류 벡터는 월드 좌표계를 기준으로 계산됨
	FVector3 GetRight() const { return WorldMatrix.Right(); }
	FVector3 GetUp() const { return WorldMatrix.Up(); }
	FVector3 GetForward() const { return WorldMatrix.Backward(); }

	void SetLocalPosition(const FVector3& Position) { LocalPosition = Position; }
	void SetLocalRotation(const FVector3& Rotation) { LocalRotation = Rotation; }
	void SetLocalScale(const FVector3& Scale) { LocalScale = Scale; }

	weak_ptr<Transform> GetParent() { return Parent; }
	void SetParent(shared_ptr<Transform> InParent) { Parent = InParent; }

private:
	// Parent 기준 위치/회전/크기
	FVector3 LocalPosition;
	FVector3 LocalRotation;
	FVector3 LocalScale;

	FMatrix LocalMatrix;	// 부모 기준 좌표계를 저장할 임시 변수
	FMatrix WorldMatrix;	// Local 좌표계를 World 좌표계로 변환하기 위한 행렬(FinalUpdate에서 수행)
							// WorldMatrix를 이용해 월드 좌표계 기준의 내 좌표는 GetWorldPosition()을 통해 획득

	weak_ptr<Transform> Parent;
};
