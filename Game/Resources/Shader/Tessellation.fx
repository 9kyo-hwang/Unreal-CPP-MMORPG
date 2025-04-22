#ifndef _TESSELLATION_FX
#define _TESSELLATION_FX

#include "Parameters.fx"

struct VSIn
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

VSOut VSMain(VSIn Input)
{
    VSOut Output = (VSOut) 0.f;
    Output.Position = Input.Position;
    Output.UV = Input.UV;
    return Output;
}

struct PatchTest
{
    float Edge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

struct HSOut
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

PatchTest ConstantHS(InputPatch<VSOut, 3> Input, int PatchID : SV_PrimitiveID)
{
    PatchTest Output = (PatchTest) 0.f;
    Output.Edge[0] = 1;
    Output.Edge[1] = 2;
    Output.Edge[2] = 3;
    Output.Inside = 1;

    return Output;
}

[domain("tri")] // 패치의 종류 (tri, quad, isoline)
[partitioning("integer")] // subdivision mode (integer 소수점 무시, fractional_even, fractional_odd)
[outputtopology("triangle_cw")] // (triangle_cw, triangle_ccw, line)
[outputcontrolpoints(3)] // 하나의 입력 패치에 대해, HS가 출력할 제어점 개수
[patchconstantfunc("ConstantHS")] // ConstantHS 함수 이름
HSOut HSMain(InputPatch<VSOut, 3> Input, int VertexID : SV_OutputControlPointID, int PatchID : SV_PrimitiveID)
{
    HSOut Output = (HSOut) 0.f;
    Output.Position = Input[VertexID].Position;
    Output.UV = Input[VertexID].UV;
    return Output;
}

struct DSOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
};

[domain("tri")]
DSOut DSMain(const OutputPatch<HSOut, 3> Input, float3 Location : SV_DomainLocation, PatchTest Patch)
{
    float3 LocalPosition = Input[0].Position * Location[0] + Input[1].Position * Location[1] + Input[2].Position * Location[2];
    float2 UV = Input[0].UV * Location[0] + Input[1].UV * Location[1] + Input[2].UV * Location[2];

    DSOut Output = (DSOut) 0.f;
    Output.Position = mul(float4(LocalPosition, 1.f), GWorldViewProjectionMatrix);
    Output.UV = UV;

    return Output;
}

float4 PSMain(DSOut Input) : SV_Target
{
    return float4(1.f, 0.f, 0.f, 1.f);
}

#endif