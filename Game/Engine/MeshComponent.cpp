#include "pch.h"
#include "MeshComponent.h"

#include "InstanceBuffer.h"
#include "Material.h"
#include "Mesh.h"
#include "SceneComponent.h"

UMeshComponent::UMeshComponent()
	: Super(EComponentType::Mesh)
{
}

UMeshComponent::~UMeshComponent() = default;

void UMeshComponent::Render()
{
	GetTransform()->PushData();	// 원래는 b1 레지스터에 TransformParameters 정보를 넘겨줬었음
	Material->PushGraphicsData();
	Mesh->Render();
}

void UMeshComponent::Render(TSharedPtr<FInstanceBuffer> InstanceBuffer)
{
	InstanceBuffer->Push();	// Instance는 VSIn 구조체의 정보를 넘겨줘야 함
	Material->PushGraphicsData();
	Mesh->Render(InstanceBuffer);
}

uint64 UMeshComponent::GetInstanceID() const
{
	if(!Mesh || !Material)
	{
		return 0;
	}

	FInstanceID InstanceID{.MeshID = Mesh->GetID(), .MaterialID = Material->GetID() };
	return InstanceID.ID;
}
