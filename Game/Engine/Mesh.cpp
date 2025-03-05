#include "pch.h"
#include "Mesh.h"

#include "Engine.h"
#include "Material.h"

void FMesh::Initialize(const vector<FVertex>& Vertices, const vector<uint32>& Indices)
{
	CreateVertexBuffer(Vertices);
	CreateIndexBuffer(Indices);
}

// CommandQueue의 RenderBegin, RenderEnd 사이에 이러한 Render들이 호출될 예정(CommandList에 들어갔으므로)
void FMesh::Render()
{
	COMMAND_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	COMMAND_LIST->IASetVertexBuffers(0, 1, &VertexBufferView);
	COMMAND_LIST->IASetIndexBuffer(&IndexBufferView);

	// TODO: 작성한 서명을 실제로 사용하려면 이 부분에서 코드 작성
	/**
	 *	ConstantBuffer에 데이터 세팅
	 *	TableDescriptorHeap에 Constant Buffer에 대응되는 CBV 전달
	 *	세팅 완료 시 TableDescriptorHeap을 Commit(Register의 Root Descriptor Table에 매핑)
	 */

	CONSTANT_BUFFER(EConstantBufferType::Transform)->Add(&Transform, sizeof(Transform));	// FConstantBuffer::Add()에서 SetShaderResourceView()를 자동적으로 수행해주고 있음.
	Material->Update();	// 이제 Material을 Update해줌

	GEngine->GetTableDescriptorHeap()->CommitTable();

	//COMMAND_LIST->DrawInstanced(VertexCount, 1, 0, 0);  // 정점 정보로만 그리는 함수
	COMMAND_LIST->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
}

void FMesh::CreateVertexBuffer(const vector<FVertex>& Vertices)
{
	VertexCount = static_cast<uint32>(Vertices.size());
	uint32 BufferSize = VertexCount * sizeof(FVertex);

	/**
	 * 원래는 Buffer를 Default, Upload 2개를 만들어서 사용
	 * GPU에서 버퍼를 활용해 물체를 그리는 용도는 Default 담당
	 * Upload는 CPU에서 작업한 걸 GPU에 넘겨주는 용도로만 사용
	 * 여기서는 편리하게 작업하기 위해 Upload 하나로 관리
	 */
	D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	DEVICE->CreateCommittedResource(	// 복사해줄 정점 정보를 저장하기 위한 GPU쪽 공간 할당
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&Desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&VertexBuffer)
	);

	// 정점 정보를 GPU, 즉 VertexBuffer에 복사
	void* VertexData = nullptr;
	CD3DX12_RANGE ReadRange(0, 0);
	VertexBuffer->Map(0, &ReadRange, &VertexData);
	::memcpy(VertexData, &Vertices[0], BufferSize);
	VertexBuffer->Unmap(0, nullptr);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = sizeof(FVertex);  // 정점 1개 크기
	VertexBufferView.SizeInBytes = BufferSize;  // 버퍼 크기
}

void FMesh::CreateIndexBuffer(const vector<uint32>& Indices)
{
	IndexCount = static_cast<uint32>(Indices.size());
	uint32 BufferSize = IndexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	DEVICE->CreateCommittedResource(	// 복사해줄 정점 정보를 저장하기 위한 GPU쪽 공간 할당
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&Desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&IndexBuffer)
	);

	void* IndexData = nullptr;
	CD3DX12_RANGE ReadRange(0, 0);
	IndexBuffer->Map(0, &ReadRange, &IndexData);
	::memcpy(IndexData, &Indices[0], BufferSize);
	IndexBuffer->Unmap(0, nullptr);

	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = BufferSize;
}
