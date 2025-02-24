#include "pch.h"
#include "Engine.h"

#include "CommandQueue.h"
#include "Device.h"
#include "SwapChain.h"

void Engine::Initialize(const FWindowInfo& InInfo)
{
	Info = InInfo;
	ResizeWindow(Info.Width, Info.Height);

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

	Device->Initialize();
	CommandQueue->Initialize(Device->GetD3DDevice(), SwapChain);
	SwapChain->Initialize(Info, Device->GetD3DDevice(), Device->GetDXGI(), CommandQueue->GetD3DCommandQueue());
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
}
