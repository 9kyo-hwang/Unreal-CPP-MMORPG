#include "pch.h"
#include "MeshRenderer.h"

#include "Material.h"
#include "Mesh.h"

FMeshRenderer::FMeshRenderer()
	: Super(EComponentType::MeshRenderer)
{
}

FMeshRenderer::~FMeshRenderer()
{
}

void FMeshRenderer::Update()
{
	//Super::Update();
	Render();
}

void FMeshRenderer::Render()
{
	// GetTransform()->Update();
	Material->Update();
	Mesh->Render();
}
