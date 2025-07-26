#include "pch.h"
#include "CoreTLS.h"

thread_local uint32 LThreadID = 0;
thread_local TStack<int32> LLockStack;