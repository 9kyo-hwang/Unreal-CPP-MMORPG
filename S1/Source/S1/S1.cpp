// Copyright Epic Games, Inc. All Rights Reserved.

#include "S1.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, S1, "S1" );

FSendBuffer::FSendBuffer(int32 InBufferSize)
{
	Buffer.SetNum(InBufferSize);
}

FSendBuffer::~FSendBuffer()
{
}

void FSendBuffer::CopyData(void* InData, int32 InLength)
{
	check(GetCapacity() >= InLength);
	::FMemory::Memcpy(Buffer.GetData(), InData, InLength);
	WriteSize = InLength;
}

void FSendBuffer::Close(uint32 InWriteSize)
{
	WriteSize = InWriteSize;
}