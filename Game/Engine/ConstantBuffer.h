#pragma once

enum class EConstantBufferType : uint8
{
	Global,
	Transform,
	Material,
	END
};

constexpr uint8 ConstantBufferCount = static_cast<uint8>(EConstantBufferType::END);

class FConstantBuffer
{
public:
	FConstantBuffer();
	~FConstantBuffer();

	void Initialize(EConstantBufferViewRegisters InRegister, uint32 ElementSize, uint32 ElementCount);
	void Clear() { Top = 0; }
	void PushGraphicsData(void* NewData, uint32 NewDataSize);
	void PushComputeData(void* NewData, uint32 NewDataSize);
	void SetGlobalGraphicsData(void* InData, uint32 InDataSize);	// 전역으로 사용될 데이터 세팅(b0 레지스터)

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(uint32 Index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 Index);

private:
	void CreateDescriptorHeap();
	void CreateDescriptor();

	// Constant Buffer
private:
	// 버퍼가 배열의 원소가 됨.
	ComPtr<ID3D12Resource> Data;	// GPU 쪽 DRAM에서 들고 있을 버퍼 요소들을 가리키는 포인터
	BYTE* MappedElement;			// CPU 쪽에서 데이터를 전송할 때 사용하는 버퍼
	uint32 Size;					// 개별 버퍼의 크기
	uint32 Count;					// 각 버퍼의 총 개수
	uint32 Top;						// 마지막 원소를 가리키는 인덱스

	// CBV들을 저장하는 Descriptor Heap
private:
	ComPtr<ID3D12DescriptorHeap> DescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle;
	uint32 IncrementSize;

	EConstantBufferViewRegisters Register;
};
