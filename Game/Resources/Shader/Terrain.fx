#ifndef _TERRAIN_FX
#define _TERRAIN_FX

#include "Parameters.fx"
#include "Utils.fx"

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

    float MinDistance = GVector2_1.x;   // 가까울 수록 높은 테셀레이션 수준
    float MaxDistance = GVector2_1.y;

    float3 Edge0Pos = (Input[1].Position + Input[2].Position) / 2.f;    // 엣지 번호는 정점의 맞은편 엣지 중앙
    float3 Edge1Pos = (Input[2].Position + Input[0].Position) / 2.f;
    float3 Edge2Pos = (Input[0].Position + Input[1].Position) / 2.f;

    Edge0Pos = mul(float4(Edge0Pos, 1.f), GWorldMatrix).xyz;    // 월드 좌표로 변환
    Edge1Pos = mul(float4(Edge1Pos, 1.f), GWorldMatrix).xyz;
    Edge2Pos = mul(float4(Edge2Pos, 1.f), GWorldMatrix).xyz;

    float Edge0Level = CalculateTessellationLevel(GVector4_0.xyz, Edge0Pos, MinDistance, MaxDistance, 4.f);
    float Edge1Level = CalculateTessellationLevel(GVector4_0.xyz, Edge1Pos, MinDistance, MaxDistance, 4.f);
    float Edge2Level = CalculateTessellationLevel(GVector4_0.xyz, Edge2Pos, MinDistance, MaxDistance, 4.f);

    Output.Edge[0] = Edge0Level;
    Output.Edge[1] = Edge1Level;
    Output.Edge[2] = Edge2Level;
    Output.Inside = Edge2Level;

    return Output;
}

[domain("tri")] // 패치의 종류 (tri, quad, isoline)
[partitioning("fractional_odd")] // subdivision mode (integer 소수점 무시, fractional_even, fractional_odd)
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

/** Terrain Shader
 *  GInt_1: Tile SizeX
 *  GInt_2: Tile SizeZ
 *  GFloat_0: Max Tessellation Level
 *  GVector2_0: HeightMap Resolution
 *  GVector2_1: Min/Max Tessellation Distance
 *  GVector4_0: Camera Position
 *  GTexture_0: Diffuse Texture
 *  GTexture_1: Normal Texture
 *  GTexture_2: HeightMap Texture
 */
struct DSOut    // 좌표 계산 필요
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
    float3 ViewPosition : POSITION;
    float3 ViewNormal : NORMAL;
    float3 ViewTangent : TANGENT;
    float3 ViewBiNormal : BINORMAL;
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