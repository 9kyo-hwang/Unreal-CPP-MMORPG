#include "pch.h"
#include "SwapChain.h"

void FSwapChain::Initialize(const FWindowInfo& Info, ComPtr<IDXGIFactory> DXGI, ComPtr<ID3D12CommandQueue> CommandQueue)
{
	SwapChain.Reset();	// 혹시 Init 함수가 2번 호출될 것을 대비해 Clear

	DXGI_MODE_DESC BufferDesc  // 버퍼 정보
	{
		.Width = static_cast<uint32>(Info.Width),	// 해상도 너비
		.Height = static_cast<uint32>(Info.Height),	// 해상도 높이
		.RefreshRate = {.Numerator = 60, .Denominator = 1},	// 주사율
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,	// 디스플레이 형식
		.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		.Scaling = DXGI_MODE_SCALING_UNSPECIFIED
	};

	DXGI_SWAP_CHAIN_DESC Desc	// 체인 정보
	{
		.BufferDesc = BufferDesc,
		.SampleDesc = {.Count = 1, .Quality = 0},	// 멀티 샘플링 OFF
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,	// Back Buffer에 그릴 것
		.BufferCount = SWAP_CHAIN_BUFFER_COUNT,	// Front Buffer, Back Buffer 2개
		.OutputWindow = Info.Window,
		.Windowed = Info.bWindowed,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,	// Buffer Swap 시 이전 프레임 정보 버리기
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
	// 현재 프레임을 그리는 것
	SwapChain->Present(0, 0);
}

void FSwapChain::SwapIndex()
{
	// 지금은 숫자가 0, 1밖에 없으므로 BackBufferIndex ^= 1; 도 가능할 듯
	BackBufferIndex = (BackBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}
