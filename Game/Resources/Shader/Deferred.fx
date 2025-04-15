#ifndef _DEFERRED_FX_
#define _DEFERRED_FX_

#include "Parameters.fx"

struct VSIn
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VSOut
{
    float4 Position : SV_Position;   // MS에서 지정한 규칙(System Value)
    float2 UV : TEXCOORD;
    float3 ViewPosition : POSITION;
    float3 ViewNormal : NORMAL;
    float3 ViewTangent : TANGENT;
    float3 ViewBinormal : BINORMAL;
};

VSOut VSMain(VSIn Input)
{
    VSOut Output = (VSOut)0;

    // 계산된 좌표값을 적용하도록 wvp_matrix를 최종적으로 곱해줌
	// 1.f: 행렬을 곱할 때 좌표로 인식, 0.f: 방향성만 추출
    Output.Position = mul(float4(Input.Position, 1.f), GWorldViewProjectionMatrix); // projection 좌표계로 넘어감
    Output.UV = Input.UV;
    Output.ViewPosition = mul(float4(Input.Position, 1.f), GWorldViewMatrix).xyz; // view 좌표계가 필요
    Output.ViewNormal = normalize(mul(float4(Input.Normal, 0.f), GWorldViewMatrix).xyz);    // translation이 적용되지 않도록 0.f 설정
    Output.ViewTangent = normalize(mul(float4(Input.Tangent, 0.f), GWorldViewMatrix).xyz);    // translation이 적용되지 않도록 0.f 설정
    Output.ViewBinormal = normalize(cross(Output.ViewTangent, Output.ViewNormal));  // tangent, normal 벡터 외적 시 binormal 등장

    return Output;
}

struct PSOut
{
    float4 Position : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Color : SV_Target2;
};

PSOut PSMain(VSOut Input)
{
    PSOut Output = (PSOut) 0;

    float4 Color = float4(1.f, 1.f, 1.f, 1.f);  // temp: 흰색으로 설정
    if (GUseTexture_0 == 1)
    {
        Color = GTexture_0.Sample(GSampler_0, Input.UV);
    }

    float3 ViewNormal = Input.ViewNormal;
	if (GUseTexture_1 == 1)
	{
        // RGB 0 ~ 255 -> 0 ~ 1 변환
        float3 TangentSpaceNormal = GTexture_1.Sample(GSampler_0, Input.UV).xyz;
        // 0 ~ 1에서 -1 ~ 1 변환
		TangentSpaceNormal = (TangentSpaceNormal - 0.5f) * 2.f;

        float3x3 TBNMatrix = { Input.ViewTangent, Input.ViewBinormal, Input.ViewNormal };
        ViewNormal = normalize(mul(TangentSpaceNormal, TBNMatrix));
    }

    Output.Position = float4(Input.ViewPosition.xyz, 0.f);  // 중간 데이터를 보관
    Output.Normal = float4(ViewNormal.xyz, 0.f);   // 중간 데이터를 보관
    Output.Color = Color;

    return Output;
}

#endif