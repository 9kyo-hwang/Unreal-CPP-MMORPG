#pragma once

enum class EObjectType : uint8
{
	None,
	GameObject,	// Prefab(Blueprint)
	Component,
	Material,
	Mesh,
	Shader,
	Texture,

	End
};

constexpr uint8 ObjectCount = static_cast<uint8>(EObjectType::End);

class Object
{
public:
	Object(EObjectType InType);
	virtual ~Object();

	EObjectType GetObjectType() const { return Type; }
	const wstring& GetName() const { return Name; }
	void SetName(const wstring& NewName) { Name = NewName; }

	// TODO: Instantiate

protected:
	friend class Resources;
	virtual void Load(const wstring& Path) {}
	virtual void Save(const wstring& Path) {}

protected:
	EObjectType Type;
	wstring Name;
};

