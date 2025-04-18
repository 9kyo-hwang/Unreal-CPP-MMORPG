#include "pch.h"
#include "Texture.h"

#include "Engine.h"

FTexture::FTexture()
	: Super(EObjectType::Texture)
	, ShaderResourceDescriptorHeapStart()
	, UnorderedAccessDescriptorHeapStart()
{
}

FTexture::~FTexture()
{
}

void FTexture::Load(const wstring& Path)
{
	wstring FileExtension = FPaths(Path).extension();
	if ( FileExtension == L".dds" || FileExtension == L".DDS" )
	{
		::LoadFromDDSFile(Path.c_str(), DDS_FLAGS_NONE, nullptr, Image);
	}
	else if ( FileExtension == L".tga" || FileExtension == L".TGA" )
	{
		::LoadFromTGAFile(Path.c_str(), nullptr, Image);
	}
	else  // png, jpg, jpeg, bmp, ...
	{
		::LoadFromWICFile(Path.c_str(), WIC_FLAGS_NONE, nullptr, Image);
	}

	assert(SUCCEEDED(::CreateTexture(DEVICE.Get(), Image.GetMetadata(), &Texture2D)));

	vector<D3D12_SUBRESOURCE_DATA> SubResources;
	assert(SUCCEEDED(::PrepareUpload(
		DEVICE.Get(),
		Image.GetImages(),
		Image.GetImageCount(),
		Image.GetMetadata(),
		SubResources
		)));

	const uint64 BufferSize = ::GetRequiredIntermediateSize(Texture2D.Get(), 0, static_cast< uint32 >( SubResources.size() ));
	D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	ComPtr<ID3D12Resource> TextureUploadHeap;
	assert(SUCCEEDED(::DEVICE->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&Desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(TextureUploadHeap.GetAddressOf()
			))));

	::UpdateSubresources(
		RESOURCE_COMMAND_LIST.Get(),
		Texture2D.Get(),
		TextureUploadHeap.Get(),
		0, 0,
		static_cast< uint32 >( SubResources.size() ),
		SubResources.data()
	);

	GEngine->GetGraphicsCommandQueue()->FlushResources();

	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = 1,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
	};
	DEVICE->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&ShaderResourceDescriptorHeap));
	ShaderResourceDescriptorHeapStart = ShaderResourceDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc
	{
		.Format = Image.GetMetadata().format,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
	};
	ViewDesc.Texture2D.MipLevels = 1;
	DEVICE->CreateShaderResourceView(Texture2D.Get(), &ViewDesc, ShaderResourceDescriptorHeapStart);
}

void FTexture::Create(DXGI_FORMAT Format, uint32 Width, uint32 Height, const D3D12_HEAP_PROPERTIES& HeapProperties,
	D3D12_HEAP_FLAGS HeapFlags, D3D12_RESOURCE_FLAGS ResourceFlags, FVector4 ClearColor)
{
	// 기존 DepthStencilBuffer에서 수행하던 작업을 여기서 수행함
	D3D12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(Format, Width, Height);
	ResourceDesc.Flags = ResourceFlags;

	D3D12_CLEAR_VALUE OptimizedClearValue{};
	D3D12_CLEAR_VALUE* OptimizedClearValuePtr = nullptr;
	D3D12_RESOURCE_STATES ResourceStates = D3D12_RESOURCE_STATE_COMMON;

	if (ResourceFlags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		ResourceStates = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		OptimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
		OptimizedClearValuePtr = &OptimizedClearValue;
	}
	else if (ResourceFlags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		ResourceStates = D3D12_RESOURCE_STATE_COMMON;
		float Color[] = { ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w };
		OptimizedClearValue = CD3DX12_CLEAR_VALUE(Format, Color);
		OptimizedClearValuePtr = &OptimizedClearValue;
	}

	assert(SUCCEEDED(DEVICE->CreateCommittedResource(
		&HeapProperties,
		HeapFlags,
		&ResourceDesc,
		ResourceStates,
		OptimizedClearValuePtr,
		IID_PPV_ARGS(&Texture2D)
	)));

	Create(Texture2D);
}

void FTexture::Create(ComPtr<ID3D12Resource> InTexture2D)
{
	Texture2D = InTexture2D;
	D3D12_RESOURCE_DESC ResourceDesc = InTexture2D->GetDesc();

	/**
	 *	1. DepthStencil 단독
	 *	2. ShaderResource(일반적인 텍스처(그려지는) 용도)
	 *	3. RenderTarget + ShaderResource
	 */

	if (ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC HeapDesc
		{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			.NumDescriptors = 1,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0,
		};
		DEVICE->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&DepthStencilDescriptorHeap));

		// 기존 DepthStencilBuffer 클래스에서 수행하던 작업을 여기서 수행
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptorHandle = DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		DEVICE->CreateDepthStencilView(Texture2D.Get(), nullptr, DepthStencilDescriptorHandle);
	}
	else
	{
		if (ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
			// 기존 SwapChain의 CreateRenderTargetView에서 수행하던 작업을 여기서 수행
			D3D12_DESCRIPTOR_HEAP_DESC HeapDesc
			{
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				.NumDescriptors = 1,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
				.NodeMask = 0,
			};
			DEVICE->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&RenderTargetDescriptorHeap));

			D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetDescriptorHandle = RenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			DEVICE->CreateRenderTargetView(Texture2D.Get(), nullptr, RenderTargetDescriptorHandle);
		}

		if (ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
		{
			D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc
			{
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				.NumDescriptors = 1,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
				.NodeMask = 0
			};
			DEVICE->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&UnorderedAccessDescriptorHeap));
			UnorderedAccessDescriptorHeapStart = UnorderedAccessDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

			D3D12_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessViewDesc{};
			{
				UnorderedAccessViewDesc.Format = Image.GetMetadata().format;
				UnorderedAccessViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			};
			DEVICE->CreateUnorderedAccessView(Texture2D.Get(), nullptr, &UnorderedAccessViewDesc, UnorderedAccessDescriptorHeapStart);
		}

		D3D12_DESCRIPTOR_HEAP_DESC HeapDesc{};
		{
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			HeapDesc.NumDescriptors = 1;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		};
		DEVICE->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&ShaderResourceDescriptorHeap));
		ShaderResourceDescriptorHeapStart = ShaderResourceDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc{};
		{
			ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			ShaderResourceViewDesc.Format = Image.GetMetadata().format;
			ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			ShaderResourceViewDesc.Texture2D.MipLevels = 1;
		}
		DEVICE->CreateShaderResourceView(Texture2D.Get(), &ShaderResourceViewDesc, ShaderResourceDescriptorHeapStart);
	}
}
