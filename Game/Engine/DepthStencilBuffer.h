#pragma once

class FDepthStencilBuffer
{
public:
	void Initialize(const FWindowInfo& Window, DXGI_FORMAT InFormat = DXGI_FORMAT_D32_FLOAT);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() { return View; }
	DXGI_FORMAT GetFormat() { return Format; }

private:
	// RenderTargetView는 Front/Back 2개를 만들었으나 DSV는 1개만 있으면 됨
	ComPtr<ID3D12Resource> Resource;
	ComPtr<ID3D12DescriptorHeap> DescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE View{};

	/**
	 *	only depth: DXGI_FORMAT_D32_FLOAT(4byte)
	 *	depth + stencil: DXGI_FORMAT_D24_UNORM_S8_UINT
	 */
	DXGI_FORMAT Format{};
};
