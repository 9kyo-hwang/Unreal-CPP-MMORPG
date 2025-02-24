#pragma once

class FMesh
{
public:
	void Initialize(const vector<struct FVertex>& Vertices);
	void Render();

private:
	ComPtr<ID3D12Resource> VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};  // Resource ����� View�� ���� �̷����
	uint32 VertexCount = 0;
};

