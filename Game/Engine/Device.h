#pragma once

// GPU���� ����� ����
class FDevice
{
public:
	void Initialize();

	ComPtr<IDXGIFactory> GetDXGI() { return DXGI; }
	ComPtr<ID3D12Device> GetDevice() { return Device; }

private:
	ComPtr<ID3D12Debug> DebugContainer;
	ComPtr<IDXGIFactory> DXGI;		// ȭ�� ���� ���
	ComPtr<ID3D12Device> Device;	// ��ü ����
};

