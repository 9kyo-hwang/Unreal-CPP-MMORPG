#pragma once
#include "Actor.h"
#include "Material.h"
#include "Mesh.h"
#include "Object.h"
#include "Texture.h"

class Resources
{
	GENERATED_SINGLETON(Resources);

public:
	void Initialize();

	template<typename T>
	TSharedPtr<T> Load(const wstring& Name, const wstring& Path);

	// TODO: 파일에서 로드하는 기능을 추가하면 삭제
	template<typename T>
	bool Add(const wstring& Name, TSharedPtr<T> NewObject);

	template<typename T>
	TSharedPtr<T> Get(const wstring& Name);

	template<typename T>
	EObjectType GetObjectType() const;

public:
	TSharedPtr<UMesh> LoadPoint();
	TSharedPtr<UMesh> LoadRectangle();
	TSharedPtr<UMesh> LoadCube();
	TSharedPtr<UMesh> LoadSphere();
	TSharedPtr<UMesh> LoadTerrain(int32 SizeX = 15, int32 SizeZ = 15);

	TSharedPtr<FTexture> CreateTexture(const wstring& Name, DXGI_FORMAT Format, uint32 Width, uint32 Height,
		const D3D12_HEAP_PROPERTIES& HeapProperties, D3D12_HEAP_FLAGS HeapFlags,
		D3D12_RESOURCE_FLAGS ResourceFlags = D3D12_RESOURCE_FLAG_NONE, FVector4 ClearColor = {});
	TSharedPtr<FTexture> CreateTexture(const wstring& Name, ComPtr<ID3D12Resource> Texture2D);

private:
	void CreateDefaultShader();
	void CreateDefaultMaterial();

private:
	using ResourceNameMap = std::unordered_map<wstring, TSharedPtr<UObject>>;
	array<ResourceNameMap, ObjectCount> ResourceList;
};

template <typename T>
TSharedPtr<T> Resources::Load(const wstring& Name, const wstring& Path)
{
	EObjectType Type = GetObjectType<T>();
	auto& Map = ResourceList[StaticCast<uint8>(Type)];

	if (Map.contains(Name))
	{
		return StaticCastSharedPtr<T>(Map[Name]);
	}

	TSharedPtr<T> NewObject = MakeShared<T>();
	NewObject->Load(Path);
	Map.emplace(Name, NewObject);

	return NewObject;
}

template <typename T>
bool Resources::Add(const wstring& Name, TSharedPtr<T> NewObject)
{
	EObjectType Type = GetObjectType<T>();
	auto& Map = ResourceList[StaticCast<uint8>(Type)];

	if (Map.contains(Name))
	{
		return true;
	}

	Map.emplace(Name, NewObject);
	return true;
}

template <typename T>
TSharedPtr<T> Resources::Get(const wstring& Name)
{
	EObjectType Type = GetObjectType<T>();
	auto& Map = ResourceList[StaticCast< uint8 >( Type )];

	if (Map.contains(Name))
	{
		return StaticCastSharedPtr<T>( Map[Name] );
	}

	return nullptr;
}

template <typename T>
EObjectType Resources::GetObjectType() const
{
	// Type trait

	if (is_same_v<T, AActor>)
	{
		return EObjectType::Actor;
	}

	if (is_same_v<T, FMaterial>)
	{
		return EObjectType::Material;
	}

	if ( is_same_v<T, UMesh> )
	{
		return EObjectType::Mesh;
	}

	if ( is_same_v<T, FShader> )
	{
		return EObjectType::Shader;
	}

	if ( is_same_v<T, FTexture> )
	{
		return EObjectType::Texture;
	}

	if ( is_same_v<T, UActorComponent> )
	{
		return EObjectType::Component;
	}

	return EObjectType::None;
}
