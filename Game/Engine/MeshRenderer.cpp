#include "pch.h"
#include "MeshRenderer.h"

#include "Material.h"
#include "Mesh.h"
#include "Transform.h"

FMeshRenderer::FMeshRenderer()
	: Super(EComponentType::MeshRenderer)
{
}

FMeshRenderer::~FMeshRenderer()
{
}

void FMeshRenderer::Render()
{
	GetTransform()->PushData();
	Material->PushData();
	Mesh->Render();
}
