#pragma once
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Object.h"

class Resources
{
	GENERATED_SINGLETON(Resources);

public:
	void Initialize();

	template<typename T>
	shared_ptr<T> Load(const wstring& Name, const wstring& Path);

	// TODO: 파일에서 로드하는 기능을 추가하면 삭제
	template<typename T>
	bool Add(const wstring& Name, shared_ptr<T> NewObject);

	template<typename T>
	shared_ptr<T> Get(const wstring& Name);

	template<typename T>
	EObjectType GetObjectType() const;

public:
	shared_ptr<FMesh> LoadRectangle();
	shared_ptr<FMesh> LoadCube();
	shared_ptr<FMesh> LoadSphere();

private:
	void CreateDefaultShader();

private:
	using ResourceNameMap = std::unordered_map<wstring, shared_ptr<Object>>;
	array<ResourceNameMap, ObjectCount> ResourceList;
};

template <typename T>
shared_ptr<T> Resources::Load(const wstring& Name, const wstring& Path)
{
	EObjectType Type = GetObjectType<T>();
	auto& Map = ResourceList[static_cast<uint8>(Type)];

	if (Map.contains(Name))
	{
		return static_pointer_cast< T >( Map[Name] );
	}

	shared_ptr<T> NewObject = make_shared<T>();
	NewObject->Load(Path);
	Map.emplace(Name, NewObject);

	return NewObject;
}

template <typename T>
bool Resources::Add(const wstring& Name, shared_ptr<T> NewObject)
{
	EObjectType Type = GetObjectType<T>();
	auto& Map = ResourceList[static_cast<uint8>(Type)];

	if (Map.contains(Name))
	{
		return true;
	}

	Map.emplace(Name, NewObject);
	return true;
}

template <typename T>
shared_ptr<T> Resources::Get(const wstring& Name)
{
	EObjectType Type = GetObjectType<T>();
	auto& Map = ResourceList[static_cast< uint8 >( Type )];

	if (Map.contains(Name))
	{
		return static_pointer_cast< T >( Map[Name] );
	}

	return nullptr;
}

template <typename T>
EObjectType Resources::GetObjectType() const
{
	// Type trait

	if (is_same_v<T, GameObject>)
	{
		return EObjectType::GameObject;
	}

	if (is_same_v<T, FMaterial>)
	{
		return EObjectType::Material;
	}

	if ( is_same_v<T, FMesh> )
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

	if ( is_same_v<T, Component> )
	{
		return EObjectType::Component;
	}

	return EObjectType::None;
}
