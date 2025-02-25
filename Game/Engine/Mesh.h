#pragma once

class FMesh
{
public:
	void Initialize(const vector<struct FVertex>& Vertices);
	void Render();

	// TEMP
	void SetTransform(const FTransform& InTransform) { Transform = InTransform; }

private:
	ComPtr<ID3D12Resource> VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};  // Resource 사용은 View를 통해 이루어짐
	uint32 VertexCount = 0;

	FTransform Transform{};
};

