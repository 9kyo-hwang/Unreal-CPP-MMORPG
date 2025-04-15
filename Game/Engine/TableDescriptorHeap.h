#pragma once

// GPU의 Register들이 알아야 할 View Array
class FGraphicsDescriptorTable
{
public:
	void Initialize(uint32 Count);
	void Clear();
	void SetConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE Src, EConstantBufferViewRegisters Register);
	void SetShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE Src, EShaderResourceViewRegisters Register);
	void Commit();

	ComPtr<ID3D12DescriptorHeap> GetD3DDescriptorHeap() { return Data; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EConstantBufferViewRegisters Register);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EShaderResourceViewRegisters Register);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 RegisterNumber);

private:
	ComPtr<ID3D12DescriptorHeap> Data;
	uint64 ViewSize = 0;

	// 일종의 이중 배열처럼, [View0, View1, ...] [View0, View1, ...] ... 와 같이 관리
	uint64 GroupSize = 0;	// == View 크기 * 그룹 내 View 개수
	uint64 GroupCount = 0;
	uint32 CurrentGroupIndex = 0;
};

class FComputeDescriptorTable
{
public:
	FComputeDescriptorTable();
	~FComputeDescriptorTable();

	void Initialize();

	void SetDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle, EConstantBufferViewRegisters Register);
	void SetDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle, EShaderResourceViewRegisters Register);
	void SetDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE SrcHandle, EUnorderedAccessViewRegisters Register);

	void Commit();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EConstantBufferViewRegisters Register);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EShaderResourceViewRegisters Register);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(EUnorderedAccessViewRegisters Register);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 Register);

	ComPtr<ID3D12DescriptorHeap> DescriptorHeap;
	uint64 IncrementSize;

	// NOTE: Group 개념을 도입할 지는 추후 적용
};