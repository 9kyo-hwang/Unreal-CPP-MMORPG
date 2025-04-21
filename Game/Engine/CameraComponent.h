#pragma once
#include "ActorComponent.h"
#include "Frustum.h"

enum class ECameraProjectionType : uint8
{
	Perspective,	// 원근
	Orthographic,	// 직교
};

class UCameraComponent : public UActorComponent
{
	using Super = UActorComponent;

public:
	UCameraComponent();
	~UCameraComponent() override;

	void FinalUpdate(float DeltaTime) override;

	void SortGameObject();
	void RenderDeferred();
	void RenderForward();

	ECameraProjectionType GetProjection() const { return Projection; }
	void SetProjection(ECameraProjectionType InProjection) { Projection = InProjection; }

	void EnableLayerCulling(uint8 Layer, bool Enable) { Enable ? CullingMask |= ( 1 << Layer ) : CullingMask &= ~( 1 << Layer ); }
	void CullAllLayers() { SetCullingMask(UINT32_MAX); }	// UINT32_MAX: 1111111... -> 모든 레이어 컬링
	void SetCullingMask(uint32 Mask) { CullingMask = Mask; }
	bool IsLayerCulled(uint8 Layer) const { return ( CullingMask & 1 << Layer ) != 0; }

private:
	ECameraProjectionType Projection;

	float Near;
	float Far;
	float Fov;
	float Scale;

	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	Frustum Frustum;

	/**
	 * 0: Culling하지 않음 <-> 1: Culling
	 * 예) UI는 레이어 1번으로 설정, [1][1][1][1][1][1][0][1]: UI만 Culling하지 않을 것. 즉 이 카메라에서는 UI만 보여줄 것. Orthographic.
	 */
	// 
	uint32 CullingMask;

private:	// 카메라로 찍어야 할 객체들을 셰이더 타입에 따라 분리해서 관리
	vector<TSharedPtr<AActor>> DeferredShaders;
	vector<TSharedPtr<AActor>> ForwardShaders;
	vector<TSharedPtr<AActor>> ParticleShaders;

public:
	// TEMP
	static FMatrix StaticViewMatrix;
	static FMatrix StaticProjectionMatrix;
};

