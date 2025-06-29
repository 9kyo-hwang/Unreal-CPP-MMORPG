#pragma once

#define CRASH(cause)						\
{											\
	uint32* Crash = nullptr;				\
	__analysis_assume(Crash != nullptr);	\
	*Crash = 0xDEADBEEF;					\
}

#define check(Expression)				\
{										\
	if(!(Expression))					\
	{									\
		CRASH("assert");				\
		__analysis_assume(Expression);	\
	}									\
}