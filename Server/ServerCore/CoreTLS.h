#pragma once
#include "Container.h"

extern thread_local uint32 LThreadID;
extern thread_local TStack<int32> LLockStack;
extern thread_local shared_ptr<class FSendBufferChunk> LSendBufferChunk;