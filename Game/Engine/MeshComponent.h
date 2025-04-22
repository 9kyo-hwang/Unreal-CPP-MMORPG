#pragma once
#include "ActorComponent.h"

class FMaterial;
class UMesh;

// 64bit를 [32][32] or [64]로 사용
union FInstanceID
{
	struct
	{
		uint32 MeshID;
		uint32 MaterialID;
	};

	uint64 ID;
};

class UMeshComponent : public UActorComponent
{
	using Super = UActorComponent;

public:
	UMeshComponent();
	~UMeshComponent() override;

	void Render();
	void Render(TSharedPtr<class FInstanceBuffer> InstanceBuffer);
	void RenderShadow();

	uint64 GetInstanceID() const;
	TSharedPtr<FMaterial> GetMaterial() { return Material;  }

	void SetMesh(TSharedPtr<UMesh> InMesh) { Mesh = InMesh; }
	void SetMaterial(TSharedPtr<FMaterial> InMaterial) { Material = InMaterial; }

private:
	TSharedPtr<UMesh> Mesh;
	TSharedPtr<FMaterial> Material;
};

