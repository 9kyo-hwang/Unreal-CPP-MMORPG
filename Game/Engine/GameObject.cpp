#include "pch.h"
#include "GameObject.h"

#include "Camera.h"
#include "MeshRenderer.h"
#include "MonoBehaviour.h"
#include "Transform.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
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

void GameObject::FinalUpdate()
{
	for ( shared_ptr<Component>& Component : Components )
	{
		if ( Component )
		{
			Component->FinalUpdate();
		}
	}

	// MonoBehaviour를 상속받는 스크립트들은 FinalUpdate를 수행하지 않음
}

shared_ptr<Component> GameObject::GetComponent(EComponentType Type) const
{
	uint8 Index = static_cast<uint8>(Type);
	assert(Index < ComponentCount);
	return Components[Index];
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

shared_ptr<Transform> GameObject::GetTransform() const
{
	return static_pointer_cast<Transform>( GetComponent(EComponentType::Transform) );
}

shared_ptr<FMeshRenderer> GameObject::GetMeshRenderer() const
{
	return static_pointer_cast< FMeshRenderer >( GetComponent(EComponentType::MeshRenderer) );
}

shared_ptr<Camera> GameObject::GetCamera() const
{
	return static_pointer_cast< Camera >( GetComponent(EComponentType::Camera) );
}
