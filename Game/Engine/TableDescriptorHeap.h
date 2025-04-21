#pragma once

// GPU의 Register들이 알아야 할 View Array
class FGraphicsResourceTables
{
public:
	FGraphicsResourceTables();
	~FGraphicsResourceTables();
	
	void Initialize(ComPtr<ID3D12Device> Device, uint32 GroupCount);
	void Clear() { Top = 0; }
	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE Src, EConstantBufferViewRegisters Register);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE Src, EShaderResourceViewRegisters Register);
	void Commit();

	ComPtr<ID3D12DescriptorHeap> GetHeap() { return Heap; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EConstantBufferViewRegisters Register);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EShaderResourceViewRegisters Register);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 Register);

private:
	ComPtr<ID3D12DescriptorHeap> Heap;
	uint64 IncrementSize;

	// 일종의 이중 배열처럼, [View0, View1, ...] [View0, View1, ...] ... 와 같이 관리
	uint64 Size;	// == View 크기 * 그룹 내 View 개수
	uint64 Count;
	uint32 Top;
};

class FComputeResourceTables
{
public:
	FComputeResourceTables();
	~FComputeResourceTables();

	void Initialize(ComPtr<ID3D12Device> Device);

	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle, EConstantBufferViewRegisters Register);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle, EShaderResourceViewRegisters Register);
	void SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle, EUnorderedAccessViewRegisters Register);

	void Commit();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EConstantBufferViewRegisters Register);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EShaderResourceViewRegisters Register);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EUnorderedAccessViewRegisters Register);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 Register);

	ComPtr<ID3D12DescriptorHeap> Heap;
	uint64 IncrementSize;

	// NOTE: Group 개념을 도입할 지는 추후 적용
};