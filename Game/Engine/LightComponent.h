#pragma once
#include "ActorComponent.h"

enum class ELightType : uint8
{
	Directional,
	Point,
	Spot,
};

struct FLightColor
{
	// RGB 외 다른 정보를 저장할 필요가 있어 FVector4
	FVector4 Diffuse;
	FVector4 Ambient;
	FVector4 Specular;
};

struct FLightInfo
{
	FLightColor Color;
	FVector4 Position;	// Directional Light에서는 사용하지 않음
	FVector4 Direction;	// Point Light에서는 사용하지 않음
	int32 Type;			// Shader쪽 Padding 처리 때문에 uint8이 아님
	float Range;		// Directional Light에서는 사용하지 않음
	float Angle;		// Spot Light에서만 사용
	int32 Padding;		// 의미없는 값. 16byte의 배수로 맞춰주기 위험
};

struct FLightParameters
{
	uint32 LightCount;
	FVector3 Padding;
	FLightInfo Lights[50];	// 다른 것과 달리 모든 빛 정보를 하나의 구조체에 다 들고 있음
};

class ULightComponent : public UActorComponent
{
	using Super = UActorComponent;

public:
	ULightComponent();
	~ULightComponent() override;

	void FinalUpdate(float DeltaTime) override;
	void Render();

public:
	const FLightInfo& GetInfo() const { return Info; }

	void SetDiffuse(const FVector3& NewDiffuse) { Info.Color.Diffuse = NewDiffuse; }
	void SetAmbient(const FVector3& NewAmbient) { Info.Color.Ambient = NewAmbient; }
	void SetSpecular(const FVector3& NewSpecular) { Info.Color.Specular = NewSpecular; }

	void SetDirection(const FVector3& NewDirection) { Info.Direction = NewDirection; }
	void SetType(ELightType NewType);
	void SetRange(float NewRange) { Info.Range = NewRange; }
	void SetAngle(float NewAngle) { Info.Angle = NewAngle; }

	void SetIndex(int8 InIndex) { Index = InIndex; }

private:
	FLightInfo Info;

	int8 Index;
	TSharedPtr<class UMesh> VolumeMesh;	// 빛이 영향을 미치는 영역
	TSharedPtr<class FMaterial> Material;	// 셰이더에 인자를 넘기기 위해 사용
};

