#include "pch.h"
#include "GameObject.h"

#include "MonoBehaviour.h"
#include "Transform.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::Initialize()
{
	AddComponent(make_shared<Transform>());
}

void GameObject::Awake()
{
	for (shared_ptr<Component>& Component : Components)
	{
		if (Component)
		{
			Component->Awake();
		}
	}

	for ( shared_ptr<MonoBehaviour>& Script : Scripts)
	{
		Script->Awake();
	}
}

void GameObject::Start()
{
	for ( shared_ptr<Component>& Component : Components )
	{
		if ( Component )
		{
			Component->Start();
		}
	}

	for ( shared_ptr<MonoBehaviour>& Script : Scripts )
	{
		Script->Start();
	}
}

void GameObject::Update()
{
	for ( shared_ptr<Component>& Component : Components )
	{
		if ( Component )
		{
			Component->Update();
		}
	}

	for ( shared_ptr<MonoBehaviour>& Script : Scripts )
	{
		Script->Update();
	}
}

void GameObject::LateUpdate()
{
	for ( shared_ptr<Component>& Component : Components )
	{
		if ( Component )
		{
			Component->LateUpdate();
		}
	}

	for ( shared_ptr<MonoBehaviour>& Script : Scripts )
	{
		Script->LateUpdate();
	}
}

shared_ptr<Transform> GameObject::GetTransform() const
{
	uint8 Index = static_cast<uint8>(EComponentType::Transform);
	return static_pointer_cast< Transform >( Components[Index] );
}

void GameObject::AddComponent(shared_ptr<Component> Component)
{
	Component->SetParent(shared_from_this());

	uint8 Index = static_cast<uint8>(Component->GetType());
	if (Index < ComponentCount)
	{
		Components[Index] = Component;
	}
	else
	{
		Scripts.push_back(dynamic_pointer_cast< MonoBehaviour >( Component ));
	}
}
