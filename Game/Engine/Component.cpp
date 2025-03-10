#include "pch.h"
#include "Component.h"

#include "GameObject.h"

Component::Component(EComponentType InType)
	: Type(InType)
{
}

Component::~Component()
{
}

void Component::Awake()
{
}

void Component::Start()
{
}

void Component::Update()
{
}

void Component::LateUpdate()
{
}

shared_ptr<GameObject> Component::GetParent() const
{
	if (!Parent.expired())
	{
		return Parent.lock();
	}

	return nullptr;
}

shared_ptr<Transform> Component::GetTransform() const
{
	if (!Parent.expired())
	{
		return Parent.lock()->GetTransform();
	}

	return nullptr;
}
