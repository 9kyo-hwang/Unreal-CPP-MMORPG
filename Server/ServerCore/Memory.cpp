#include "pch.h"
#include "Memory.h"

void* FMalloc::Malloc(size_t Count)
{
	return ::malloc(Count);
}

void FMalloc::Free(void* Original)
{
	::free(Original);
}
