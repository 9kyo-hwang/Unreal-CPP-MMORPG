#pragma once
#include "Object.h"

class FMaterial;

class UMesh : public UObject
{
	using Super = UObject;

public:
	UMesh();
	~UMesh() override;

	void Initialize(const vector<FVertex>& Vertices, const vector<uint32>& Indices);
	void Render(uint32 InstanceCount = 1);
	void Render(TSharedPtr<class FInstanceBuffer>& InstanceBuffer);

private:
	void CreateVertexBuffer(const vector<FVertex>& Vertices);
	void CreateIndexBuffer(const vector<uint32>& Indices);

private:
	ComPtr<ID3D12Resource> VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;  // Resource 사용은 View를 통해 이루어짐
	uint32 NumVertices;

	ComPtr<ID3D12Resource> IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	uint32 NumIndices;
};
