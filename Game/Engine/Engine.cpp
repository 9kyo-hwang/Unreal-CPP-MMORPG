#include "pch.h"
#include "Engine.h"

#include "InputManager.h"
#include "Material.h"
#include "SceneManager.h"
#include "TimeManager.h"
#include "Transform.h"

Engine::Engine()
	: Info()
	, Viewport()
	, ScissorRect()
{
	Device = make_shared<FDevice>();
	CommandQueue = make_shared<FCommandQueue>();
	SwapChain = make_shared<FSwapChain>();
	RootSignature = make_shared<FRootSignature>();
	TableDescriptorHeap = make_shared<FTableDescriptorHeap>();
	DepthStencilBuffer = make_shared<FDepthStencilBuffer>();
}

Engine::~Engine()
{
}

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

	Device->Initialize();
	CommandQueue->Initialize(SwapChain);
	SwapChain->Initialize(Info, Device->GetDXGI(), CommandQueue->GetD3DCommandQueue());
	RootSignature->Initialize();

	CreateConstantBuffer(EConstantBufferViewRegisters::b0, sizeof(FTransformParameters), 256);
	CreateConstantBuffer(EConstantBufferViewRegisters::b1, sizeof(FMaterialParameters), 256);

	TableDescriptorHeap->Initialize(256);
	DepthStencilBuffer->Initialize(Info);

	InputManager::Get()->Initialize(Info.Window);
	TimeManager::Get()->Initialize();

	ResizeWindow(Info.Width, Info.Height);  // DepthStencilBuffer init 과정 때문에 DEVICE 생성 후 Resize 하도록 조정
}

void Engine::Update()
{
	InputManager::Get()->Update();
	TimeManager::Get()->Update();
	SceneManager::Get()->Update();

	Render();

	ShowFPS();
}

void Engine::Render()
{
	RenderBegin();

	SceneManager::Get()->Render();

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

void Engine::ShowFPS()
{
	WCHAR Text[100] = L"";
	::wsprintf(Text, L"FPS: %d", TimeManager::Get()->GetFPS());

	::SetWindowText(Info.Window, Text);
}

void Engine::CreateConstantBuffer(EConstantBufferViewRegisters Register, uint32 BufferSize, uint32 Count)
{
	assert(ConstantBufferList.size() == static_cast<uint8>(Register));

	shared_ptr<FConstantBuffer> ConstantBuffer = make_shared<FConstantBuffer>();
	ConstantBuffer->Initialize(Register, BufferSize, Count);
	ConstantBufferList.push_back(ConstantBuffer);
}
