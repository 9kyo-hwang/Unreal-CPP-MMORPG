#pragma once

// GPU의 Register들이 알아야 할 View Array
class FTableDescriptorHeap
{
public:
	void Initialize(uint32 Count);
	void Clear();
	void SetConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE Src, EConstantBufferViewRegisters Register);
	void SetShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE Src, EShaderResourceViewRegisters Register);
	void CommitTable();

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

