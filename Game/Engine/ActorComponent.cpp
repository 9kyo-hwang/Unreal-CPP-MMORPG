#include "pch.h"
#include "ActorComponent.h"

#include "Actor.h"

UActorComponent::UActorComponent(EComponentType InType)
	: Super(EObjectType::Component)
	, ComponentType(InType)
{
}

UActorComponent::~UActorComponent() = default;

TSharedPtr<AActor> UActorComponent::GetOwner() const
{
	if (!Owner.expired())
	{
		return Owner.lock();
	}

	return nullptr;
}

TSharedPtr<USceneComponent> UActorComponent::GetTransform() const
{
	if (!Owner.expired())
	{
		return Owner.lock()->GetSceneComponent();
	}

	return nullptr;
}
