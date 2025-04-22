#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "Parameters.fx"

struct VSIn
{
    float3 Position : POSITION;
};

struct VSOut
{
    float4 Position : SV_Position;
    float4 ClipPosition : POSITION;
};

VSOut VSMain(VSIn Input)
{
    VSOut Output = (VSOut) 0;
    Output.Position = mul(float4(Input.Position, 1.f), GWorldViewProjectionMatrix);
    Output.ClipPosition = Output.Position;
    return Output;
}

float4 PSMain(VSOut Input) : SV_Target
{
    return float4(Input.ClipPosition.z / Input.ClipPosition.w, 0.f, 0.f, 0.f);
}

#endif