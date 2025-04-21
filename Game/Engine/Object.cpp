#include "pch.h"
#include "Object.h"

UObject::UObject(EObjectType InType)
	: ObjectType(InType)
{
	static uint32 IDGenerator = 1;
	ID = IDGenerator++;
}

UObject::~UObject() = default;
