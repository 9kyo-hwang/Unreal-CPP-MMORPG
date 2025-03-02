#pragma once

class FConstantBuffer
{
public:
	FConstantBuffer();
	~FConstantBuffer();

	void Initialize(uint32 Size, uint32 Count);
	void Clear() { CurrentIndex = 0; }
	D3D12_CPU_DESCRIPTOR_HANDLE Add(int32 RootParameterIndex, void* InData, uint32 InDataSize);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(uint32 Index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 Index);

private:
	void CreateBuffer();
	void CreateView();

	uint32 Num() const { return ElementCount; }
	uint32 GetTypeSize() const { return ElementSize; }
	bool IsValidIndex(int32 Index) const { return 0 <= Index && Index < ElementCount; }

	// Constant Buffer
private:
	// 버퍼가 배열의 원소가 됨.
	ComPtr<ID3D12Resource> Data;	// GPU 쪽 DRAM에서 들고 있을 버퍼 요소들을 가리키는 포인터
	BYTE* MappedElement = nullptr;	// CPU 쪽에서 데이터를 전송할 때 사용하는 버퍼
	uint32 ElementSize = 0;			// 개별 버퍼의 크기
	uint32 ElementCount = 0;		// 각 버퍼의 총 개수
	uint32 CurrentIndex = 0;		// 마지막 원소를 가리키는 인덱스

	// CBV들을 저장하는 Descriptor Heap
private:
	ComPtr<ID3D12DescriptorHeap> ConstantBufferViewList;
	D3D12_CPU_DESCRIPTOR_HANDLE ListBegin{};
	uint32 ListOffset = 0;
};
