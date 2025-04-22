#include "pch.h"
#include "Engine.h"

#include "InputManager.h"
#include "InstanceManager.h"
#include "LightComponent.h"
#include "Material.h"
#include "Resources.h"
#include "SceneManager.h"
#include "TimeManager.h"

Engine::Engine()
	: Info()
	, Viewport()
	, ScissorRect()
{
	Device					= MakeShared<FDevice>();
	GraphicsCommandQueue	= MakeShared<FGraphicsCommandQueue>();
	ComputeCommandQueue		= MakeShared<FComputeCommandQueue>();
	SwapChain				= MakeShared<FSwapChain>();
	GraphicsRootSignature	= MakeShared<FGraphicsRootSignature>();
	ComputeRootSignature	= MakeShared<FComputeRootSignature>();
	GraphicsResourceTables	= MakeShared<FGraphicsResourceTables>();
	ComputeResourceTables	= MakeShared<FComputeResourceTables>();
}

Engine::~Engine() = default;

void Engine::Initialize(const FWindowInfo& InInfo)
{
	Info = InInfo;

	Viewport =
	{
		.TopLeftX = 0, .TopLeftY = 0,
		.Width = StaticCast<FLOAT>(Info.Width),
		.Height = StaticCast<FLOAT>(Info.Height),
		.MinDepth = 0.0f, .MaxDepth = 1.0f
	};

	ScissorRect = CD3DX12_RECT(0, 0, Info.Width, Info.Height);

	Device->Initialize();
	GraphicsCommandQueue->Initialize(Device->GetDevice(), SwapChain);
	ComputeCommandQueue->Initialize(Device->GetDevice());
	SwapChain->Initialize(Info, Device->GetFactory(), GraphicsCommandQueue->GetCommandQueue());
	GraphicsRootSignature->Initialize(Device->GetDevice(), GraphicsCommandQueue->GetCommandList());
	ComputeRootSignature->Initialize(Device->GetDevice(), ComputeCommandQueue->GetCommandList());

	CreateConstantBuffer(EConstantBufferViewRegisters::b0, sizeof(FLightParameters), 1);	// 1개만 세팅하므로 버퍼 1개만
	CreateConstantBuffer(EConstantBufferViewRegisters::b1, sizeof(FTransformParameters), 256);
	CreateConstantBuffer(EConstantBufferViewRegisters::b2, sizeof(FMaterialParameters), 256);

	CreateMultipleRenderTargets();

	GraphicsResourceTables->Initialize(Device->GetDevice(), 256);
	ComputeResourceTables->Initialize(Device->GetDevice());

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
	UInstanceManager::Get()->ClearBuffer();

	Render();

	ShowFPS();
}

void Engine::Render()
{
	PreRender();

	SceneManager::Get()->Render();

	PostRender();
}

void Engine::PreRender()
{
	GraphicsCommandQueue->PreRender();
}

void Engine::PostRender()
{
	GraphicsCommandQueue->PostRender();
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
	assert(ConstantBufferList.size() == StaticCast<uint8>(Register));

	TSharedPtr<FConstantBuffer> ConstantBuffer = MakeShared<FConstantBuffer>();
	ConstantBuffer->Initialize(Register, BufferSize, Count);
	ConstantBufferList.push_back(ConstantBuffer);
}

void Engine::CreateMultipleRenderTargets()
{
	TSharedPtr<FTexture> DepthStencilTexture = Resources::Get()->CreateTexture(
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
			RenderTargets[Index].Texture = Resources::Get()->CreateTexture(Name, Resource);
		}

		ERenderTargetType Type = ERenderTargetType::SwapChain;
		MultipleRenderTargetArray[StaticCast<uint8>(Type)] = MakeShared<FMultipleRenderTarget>();
		MultipleRenderTargetArray[StaticCast<uint8>(Type)]->Create(Device->GetDevice(), Type, RenderTargets, DepthStencilTexture);
	}

	// Shadow
	{
		vector<FRenderTarget> RenderTargets(NumRenderTargetShadowMember);
		RenderTargets[0].Texture = Resources::Get()->CreateTexture(
			L"ShadowTarget",
			DXGI_FORMAT_R32_FLOAT,	// Red Channel Only
			4096, 4096,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		TSharedPtr<FTexture> ShadowDepthTexture = Resources::Get()->CreateTexture(
			L"ShadowDepthStencil",	// 별도의 DepthStencil 사용(공유 X, 별도 크기를 가지기 때문)
			DXGI_FORMAT_D32_FLOAT,
			4096, 4096,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

		ERenderTargetType Type = ERenderTargetType::Shadow;
		MultipleRenderTargetArray[StaticCast<uint8>(Type)] = MakeShared<FMultipleRenderTarget>();
		MultipleRenderTargetArray[StaticCast<uint8>(Type)]->Create(Device->GetDevice(), Type, RenderTargets, ShadowDepthTexture);
	}

	// Deferred
	{
		vector<FRenderTarget> RenderTargets(NumRenderTargetGeometryBufferMember);
		RenderTargets[0].Texture = Resources::Get()->CreateTexture(
			L"PositionTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		RenderTargets[1].Texture = Resources::Get()->CreateTexture(
			L"NormalTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		RenderTargets[2].Texture = Resources::Get()->CreateTexture(
			L"DiffuseTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		ERenderTargetType Type = ERenderTargetType::GeometryBuffer;
		MultipleRenderTargetArray[StaticCast<uint8>(Type)] = MakeShared<FMultipleRenderTarget>();
		MultipleRenderTargetArray[StaticCast<uint8>(Type)]->Create(Device->GetDevice(), Type, RenderTargets, DepthStencilTexture);
	}

	// Lighting
	{
		vector<FRenderTarget> RenderTargets(NumRenderTargetLightingMember);
		RenderTargets[0].Texture = Resources::Get()->CreateTexture(
			L"DiffuseLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		RenderTargets[1].Texture = Resources::Get()->CreateTexture(
			L"SpecularLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			Info.Width, Info.Height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		ERenderTargetType Type = ERenderTargetType::Lighting;
		MultipleRenderTargetArray[StaticCast<uint8>(Type)] = MakeShared<FMultipleRenderTarget>();
		MultipleRenderTargetArray[StaticCast<uint8>(Type)]->Create(Device->GetDevice(), Type, RenderTargets, DepthStencilTexture);
	}
}
