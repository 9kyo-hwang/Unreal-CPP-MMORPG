#pragma once

enum class EObjectType : uint8
{
	None,
	Actor,	// Prefab(Blueprint)
	Component,
	Material,
	Mesh,
	Shader,
	Texture,

	End
};

constexpr uint8 ObjectCount = ConstexprCast<uint8>(EObjectType::End);

class UObject
{
public:
	UObject(EObjectType InType);
	virtual ~UObject();

	EObjectType GetObjectType() const { return ObjectType; }
	const wstring& GetName() const { return Name; }
	void SetName(const wstring& NewName) { Name = NewName; }
	uint32 GetID() const { return ID; }

	// TODO: Instantiate

protected:
	friend class Resources;
	virtual void Load(const wstring& Path) {}
	virtual void Save(const wstring& Path) {}

protected:
	EObjectType ObjectType;
	wstring Name;
	uint32 ID;
};

