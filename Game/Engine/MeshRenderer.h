#pragma once
#include "Component.h"

class FMaterial;
class FMesh;

class FMeshRenderer : public Component
{
	using Super = Component;

public:
	FMeshRenderer();
	~FMeshRenderer() override;

	void Render();

	void SetMesh(shared_ptr<FMesh> InMesh) { Mesh = InMesh; }
	void SetMaterial(shared_ptr<FMaterial> InMaterial) { Material = InMaterial; }

private:
	shared_ptr<FMesh> Mesh;
	shared_ptr<FMaterial> Material;
};

