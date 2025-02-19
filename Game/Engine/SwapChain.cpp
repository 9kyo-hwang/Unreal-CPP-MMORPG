#include "pch.h"
#include "SwapChain.h"

void FSwapChain::Initialize(const FWindowInfo& Info, ComPtr<IDXGIFactory> DXGI, ComPtr<ID3D12CommandQueue> CommandQueue)
{
	SwapChain.Reset();	// Ȥ�� Init �Լ��� 2�� ȣ��� ���� ����� Clear

	DXGI_MODE_DESC BufferDesc  // ���� ����
	{
		.Width = static_cast<uint32>(Info.Width),	// �ػ� �ʺ�
		.Height = static_cast<uint32>(Info.Height),	// �ػ� ����
		.RefreshRate = {.Numerator = 60, .Denominator = 1},	// �ֻ���
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,	// ���÷��� ����
		.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		.Scaling = DXGI_MODE_SCALING_UNSPECIFIED
	};

	DXGI_SWAP_CHAIN_DESC Desc	// ü�� ����
	{
		.BufferDesc = BufferDesc,
		.SampleDesc = {.Count = 1, .Quality = 0},	// ��Ƽ ���ø� OFF
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,	// Back Buffer�� �׸� ��
		.BufferCount = SWAP_CHAIN_BUFFER_COUNT,	// Front Buffer, Back Buffer 2��
		.OutputWindow = Info.Window,
		.Windowed = Info.bWindowed,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,	// Buffer Swap �� ���� ������ ���� ������
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	DXGI->CreateSwapChain(CommandQueue.Get(), &Desc, &SwapChain);

	for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		SwapChain->GetBuffer(i, IID_PPV_ARGS(&RenderTargets[i]));
	}
}

void FSwapChain::Present() const
{
	// ���� �������� �׸��� ��
	SwapChain->Present(0, 0);
}

void FSwapChain::SwapIndex()
{
	// ������ ���ڰ� 0, 1�ۿ� �����Ƿ� BackBufferIndex ^= 1; �� ������ ��
	BackBufferIndex = (BackBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}
