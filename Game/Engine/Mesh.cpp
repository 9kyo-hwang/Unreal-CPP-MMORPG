#include "pch.h"
#include "Mesh.h"

#include "Engine.h"

void FMesh::Initialize(const vector<FVertex>& Vertices)
{
	VertexCount = static_cast<uint32>(Vertices.size());
	uint32 BufferSize = VertexCount * sizeof(FVertex);

	/**
	 * ������ Buffer�� Default, Upload 2���� ���� ���
	 * GPU���� ���۸� Ȱ���� ��ü�� �׸��� �뵵�� Default ���
	 * Upload�� CPU���� �۾��� �� GPU�� �Ѱ��ִ� �뵵�θ� ���
	 * ���⼭�� ���ϰ� �۾��ϱ� ���� Upload �ϳ��� ����
	 */
	D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	DEVICE->CreateCommittedResource(	// �������� ���� ������ �����ϱ� ���� GPU�� ���� �Ҵ�
		&HeapProperties, 
		D3D12_HEAP_FLAG_NONE, 
		&Desc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, 
		IID_PPV_ARGS(&VertexBuffer)
	);

	// ���� ������ GPU, �� VertexBuffer�� ����
	void* VertexData = nullptr;
	CD3DX12_RANGE ReadRange(0, 0);
	VertexBuffer->Map(0, &ReadRange, &VertexData);
	::memcpy(VertexData, &Vertices[0], BufferSize);
	VertexBuffer->Unmap(0, nullptr);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = sizeof(FVertex);  // ���� 1�� ũ��
	VertexBufferView.SizeInBytes = BufferSize;  // ���� ũ��
}

// CommandQueue�� RenderBegin, RenderEnd ���̿� �̷��� Render���� ȣ��� ����(CommandList�� �����Ƿ�)
void FMesh::Render()
{
	COMMAND_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	COMMAND_LIST->IASetVertexBuffers(0, 1, &VertexBufferView);
	COMMAND_LIST->DrawInstanced(VertexCount, 1, 0, 0);
}
