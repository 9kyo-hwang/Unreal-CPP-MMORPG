#pragma once

// GPU와의 통신을 수행
class FDevice
{
public:
	void Initialize();

	ComPtr<IDXGIFactory> GetFactory() { return DXGIFactory; }
	ComPtr<ID3D12Device> GetDevice() { return Direct3DDevice; }

private:
	ComPtr<ID3D12Debug> DebugContainer;
	ComPtr<IDXGIFactory> DXGIFactory;			// 화면 관련 기능
	ComPtr<ID3D12Device> Direct3DDevice;		// 객체 생성
};

