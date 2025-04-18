#pragma once
#include "Object.h"

class FMaterial;

class FMesh : public Object
{
	using Super = Object;

public:
	FMesh();
	~FMesh() override;

	void Initialize(const vector<FVertex>& Vertices, const vector<uint32>& Indices);
	void Render(uint32 InstanceCount = 1);

private:
	void CreateVertexBuffer(const vector<FVertex>& Vertices);
	void CreateIndexBuffer(const vector<uint32>& Indices);

private:
	ComPtr<ID3D12Resource> VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferDescriptor;  // Resource 사용은 View를 통해 이루어짐
	uint32 NumVertices;

	ComPtr<ID3D12Resource> IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW IndexBufferDescriptor;
	uint32 NumIndices;
};
