// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct PacketHeader
{
	uint16 Size;
	uint16 Id;
};

class FSendBuffer : public TSharedFromThis<FSendBuffer>
{
public:
	FSendBuffer(int32 InBufferSize);
	~FSendBuffer();

	BYTE* GetData() { return Buffer.GetData(); }
	int32 GetWriteSize() const { return WriteSize; }
	int32 GetCapacity() const { return Buffer.Num(); }

	void CopyData(void* InData, int32 InLength);
	void Close(uint32 InWriteSize);

private:
	TArray<BYTE>	Buffer;
	int32			WriteSize = 0;
};

#define USING_SHARED_PTR(name) using name##Ref = TSharedPtr<class name>;
USING_SHARED_PTR(FPacketSession);
USING_SHARED_PTR(FSendBuffer);