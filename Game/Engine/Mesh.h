#pragma once

class FTexture;

class FMesh
{
public:
	void Initialize(const vector<FVertex>& Vertices, const vector<uint32>& Indices);
	void Render();

	// TEMP
	void SetTransform(const FTransform& InTransform) { Transform = InTransform; }
	void SetTexture(shared_ptr<FTexture> InTexture) { Texture = InTexture; }

private:
	void CreateVertexBuffer(const vector<FVertex>& Vertices);
	void CreateIndexBuffer(const vector<uint32>& Indices);

private:
	ComPtr<ID3D12Resource> VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};  // Resource 사용은 View를 통해 이루어짐
	uint32 VertexCount = 0;

	ComPtr<ID3D12Resource> IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView{};
	uint32 IndexCount = 0;

	FTransform Transform{};
	shared_ptr<FTexture> Texture{};  // TEMP
};
