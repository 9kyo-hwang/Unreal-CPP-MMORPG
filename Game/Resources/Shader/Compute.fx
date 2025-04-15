#ifndef _COMPUTE_FX_
#define _COMPUTE_FX_

#include "Parameters.fx"

// Read, Write ��� ������ Texture. ���������� Read-Only
RWTexture2D<float4> GRWTexture2D_0 : register(u0);  // UAV(unordered access view)

/**
 *  X: �׸��� �� ��� ����, Y/Z: ��� �� ������ ����(2����)
 *  max: 1024(CS_5.0)
 *  �ϳ��� ������ �׷��� �ϳ��� ���� ó���⿡�� ����
 *  SV_DispatchThreadID: �ش� �������� Unique ID(Grid/Block/Cell ��� �ݿ���)
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