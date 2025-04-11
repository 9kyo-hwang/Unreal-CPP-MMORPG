#include "pch.h"
#include "Texture.h"

#include "Engine.h"

FTexture::FTexture()
	: Super(EObjectType::Texture)
{
}

FTexture::~FTexture()
{
}

void FTexture::Load(const wstring& Path)
{
	CreateTexture(Path);
	CreateView();
}

void FTexture::CreateTexture(const wstring& Path)
{
	wstring FileExtension = FPaths(Path).extension();
	if (FileExtension == L".dds" || FileExtension == L".DDS")
	{
		::LoadFromDDSFile(Path.c_str(), DDS_FLAGS_NONE, nullptr, Image);
	}
	else if (FileExtension == L".tga" || FileExtension == L".TGA")
	{
		::LoadFromTGAFile(Path.c_str(), nullptr, Image);
	}
	else  // png, jpg, jpeg, bmp, ...
	{
		::LoadFromWICFile(Path.c_str(), WIC_FLAGS_NONE, nullptr, Image);
	}

	if (FAILED(::CreateTexture(DEVICE.Get(), Image.GetMetadata(), &Texture2D)))
	{
		assert(nullptr && "FTexture::CreateTexture() - CreateTexture Failed");
	}

	vector<D3D12_SUBRESOURCE_DATA> SubResources;
	if (FAILED(::PrepareUpload(DEVICE.Get(), 
		Image.GetImages(), 
		Image.GetImageCount(), 
		Image.GetMetadata(), 
		SubResources))
		)
	{
		assert(nullptr && "FTexture::CreateTexture() - PrepareUpload Failed");
	}

	const uint64 BufferSize = ::GetRequiredIntermediateSize(Texture2D.Get(), 0, static_cast<uint32>(SubResources.size()));
	D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	ComPtr<ID3D12Resource> TextureUploadHeap;
	if (FAILED(::DEVICE->CreateCommittedResource(
		&HeapProperties, 
		D3D12_HEAP_FLAG_NONE, 
		&Desc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, 
		IID_PPV_ARGS(TextureUploadHeap.GetAddressOf())))
		)
	{
		assert(nullptr && "FTexture::CreateTexture() - CreateCommittedResource Failed");
	}

	::UpdateSubresources(
		RESOURCE_COMMAND_LIST.Get(), 
		Texture2D.Get(), 
		TextureUploadHeap.Get(), 
		0, 0, 
		static_cast<uint32>(SubResources.size()), 
		SubResources.data()
	);

	GEngine->GetCommandQueue()->FlushResourceCommandQueue();
}

void FTexture::CreateView()
{
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = 1,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
	};

	DEVICE->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&ShaderResourceViewList));

	ViewHandle = ShaderResourceViewList->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc
	{
		.Format = Image.GetMetadata().format,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
	};
	ViewDesc.Texture2D.MipLevels = 1;

	DEVICE->CreateShaderResourceView(Texture2D.Get(), &ViewDesc, ViewHandle);
}
