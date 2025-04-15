#include "pch.h"
#include "Engine.h"

#include "InputManager.h"
#include "Light.h"
#include "Material.h"
#include "Resources.h"
#include "SceneManager.h"
#include "TimeManager.h"

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

	CreateConstantBuffer(EConstantBufferViewRegisters::b0, sizeof(FLightParameters), 1);	// 1개만 세팅하므로 버퍼 1개만
	CreateConstantBuffer(EConstantBufferViewRegisters::b1, sizeof(FTransformParameters), 256);
	CreateConstantBuffer(EConstantBufferViewRegisters::b2, sizeof(FMaterialParameters), 256);

	CreateMultipleRenderTargets();

	TableDescriptorHeap->Initialize(256);

	InputManager::Get()->Initialize(Info.Window);
	TimeManager::Get()->Initialize();
	Resources::Get()->Initialize();

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

void Engine::CreateMultipleRenderTargets()
{
	shared_ptr<FTexture> DepthStencilTexture = Resources::Get()->CreateTexture(
		L"DepthStencil",
		DXGI_FORMAT_D32_FLOAT,
		Info.Width, Info.Height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	// SwapChain
	{
		vector<FRenderTarget> RenderTargets(NumSwapChainBuffer);
		for (uint32 Index = 0; Index < NumSwapChainBuffer; ++Index)
		{
			wstring Name = L"SwapChainTarget_" + std::to_wstring(Index);
			ComPtr<ID3D12Resource> Resource;

			// 원래 SwapChain Create 단계에서 내부적으로 버퍼 세팅을 진행했었음
			SwapChain->GetDXGISwapChain()->GetBuffer(Index, IID_PPV_ARGS(&Resource));
			RenderTargets[Index].Target = Resources::Get()->CreateTexture(Name, Resource);
		}

		EMultipleRenderTargetType Type = EMultipleRenderTargetType::SwapChain;
		MultipleRenderTargetArray[static_cast<uint8>(Type)] = make_shared<MultipleRenderTarget>();
		MultipleRenderTargetArray[static_cast<uint8>(Type)]->Create(Type, RenderTargets, DepthStencilTexture);
	}

	// Deferred
	{
		vector<FRenderTarget> RenderTargets(NumRenderTargetGeometryBufferMember);
		RenderTargets[0].Target = Resources::Get()->CreateTexture(
			L"PositionTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		RenderTargets[1].Target = Resources::Get()->CreateTexture(
			L"NormalTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		RenderTargets[2].Target = Resources::Get()->CreateTexture(
			L"DiffuseTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		EMultipleRenderTargetType Type = EMultipleRenderTargetType::GeometryBuffer;
		MultipleRenderTargetArray[static_cast<uint8>(Type)] = make_shared<MultipleRenderTarget>();
		MultipleRenderTargetArray[static_cast<uint8>(Type)]->Create(Type, RenderTargets, DepthStencilTexture);
	}

	// Lighting
	{
		vector<FRenderTarget> RenderTargets(NumRenderTargetLightingMember);
		RenderTargets[0].Target = Resources::Get()->CreateTexture(
			L"DiffuseLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		RenderTargets[1].Target = Resources::Get()->CreateTexture(
			L"SpecularLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		EMultipleRenderTargetType Type = EMultipleRenderTargetType::Lighting;
		MultipleRenderTargetArray[static_cast<uint8>(Type)] = make_shared<MultipleRenderTarget>();
		MultipleRenderTargetArray[static_cast<uint8>(Type)]->Create(Type, RenderTargets, DepthStencilTexture);
	}
}
