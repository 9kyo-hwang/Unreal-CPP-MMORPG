#pragma once

// GPU와의 통신을 수행
class FDevice
{
public:
	void Initialize();

	ComPtr<IDXGIFactory> GetDXGI() { return DXGI; }
	ComPtr<ID3D12Device> GetDevice() { return Device; }

private:
	ComPtr<ID3D12Debug> DebugContainer;
	ComPtr<IDXGIFactory> DXGI;		// 화면 관련 기능
	ComPtr<ID3D12Device> Device;	// 객체 생성
};

