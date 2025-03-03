#include "pch.h"
#include "Engine.h"

void Engine::Initialize(const FWindowInfo& InInfo)
{
	Info = InInfo;

	Viewport =
	{
		.TopLeftX = 0, .TopLeftY = 0,
		.Width = static_cast<FLOAT>(Info.Width),
		.Height = static_cast<FLOAT>(Info.Height),
		.MinDepth = 0.0f, .MaxDepth = 1.0f
	};

	ScissorRect = CD3DX12_RECT(0, 0, Info.Width, Info.Height);

	Device = make_shared<FDevice>();
	CommandQueue = make_shared<FCommandQueue>();
	SwapChain = make_shared<FSwapChain>();
	RootSignature = make_shared<FRootSignature>();
	ConstantBuffer = make_shared<FConstantBuffer>();
	TableDescriptorHeap = make_shared<FTableDescriptorHeap>();
	DepthStencilBuffer = make_shared<FDepthStencilBuffer>();

	Device->Initialize();
	CommandQueue->Initialize(SwapChain);
	SwapChain->Initialize(Info, Device->GetDXGI(), CommandQueue->GetD3DCommandQueue());
	RootSignature->Initialize();
	ConstantBuffer->Initialize(sizeof(FTransform), 256);	// 보통 개수는 100단위를 넘기지 않음
	TableDescriptorHeap->Initialize(256);
	DepthStencilBuffer->Initialize(Info);

	ResizeWindow(Info.Width, Info.Height);  // DepthStencilBuffer init 과정 때문에 DEVICE 생성 후 Resize 하도록 조정
}

void Engine::Render()
{
	RenderBegin();

	// TODO: 물체를 그려야 함

	RenderEnd();
}

void Engine::RenderBegin()
{
	CommandQueue->RenderBegin(&Viewport, &ScissorRect);
}

void Engine::RenderEnd()
{
	CommandQueue->RenderEnd();
}

void Engine::ResizeWindow(const int32 Width, const int32 Height)
{
	Info.Width = Width;
	Info.Height = Height;

	RECT Rect{ .left = 0, .top = 0, .right = Width, .bottom = Height };
	::AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, false);
	::SetWindowPos(Info.Window, nullptr, 100, 100, Width, Height, 0);

	// 해상도와 동일한 크기의 버퍼를 생성하므로 이 과정이 필요함
	DepthStencilBuffer->Initialize(Info);
}
