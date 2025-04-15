#ifndef _COMPUTE_FX_
#define _COMPUTE_FX_

#include "Parameters.fx"

// Read, Write 모두 가능한 Texture. 기존까지는 Read-Only
RWTexture2D<float4> GRWTexture2D_0 : register(u0);  // UAV(unordered access view)

/**
 *  X: 그리드 내 블록 개수, Y/Z: 블록 내 쓰레드 개수(2차원)
 *  max: 1024(CS_5.0)
 *  하나의 쓰레드 그룹은 하나의 다중 처리기에서 실행
 *  SV_DispatchThreadID: 해당 쓰레드의 Unique ID(Grid/Block/Cell 모두 반영한)
 */
[numthreads(1024, 1, 1)]
void CSMain(int3 ThreadIndex : SV_DispatchThreadID)
{
	if (ThreadIndex.y % 2 == 0)
	{
        GRWTexture2D_0[ThreadIndex.xy] = float4(1.f, 0.f, 0.f, 1.f);
    }
	else
	{
        GRWTexture2D_0[ThreadIndex.xy] = float4(0.f, 1.f, 0.f, 1.f);
    }
}

#endif