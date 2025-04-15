#ifndef _DEFAULT_FX_
#define _DEFAULT_FX_

#include "Parameters.fx"
#include "Utils.fx"

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

VSOut VSMain(VSIn input)
{
    VSOut output = (VSOut)0;

    // 계산된 좌표값을 적용하도록 wvp_matrix를 최종적으로 곱해줌
	// 1.f: 행렬을 곱할 때 좌표로 인식, 0.f: 방향성만 추출
    output.Position = mul(float4(input.Position, 1.f), GWorldViewProjectionMatrix); // projection 좌표계로 넘어감
    output.UV = input.UV;
    output.ViewPosition = mul(float4(input.Position, 1.f), GWorldViewMatrix).xyz; // view 좌표계가 필요
    output.ViewNormal = normalize(mul(float4(input.Normal, 0.f), GWorldViewMatrix).xyz);    // translation이 적용되지 않도록 0.f 설정
    output.ViewTangent = normalize(mul(float4(input.Tangent, 0.f), GWorldViewMatrix).xyz);    // translation이 적용되지 않도록 0.f 설정
    output.ViewBinormal = normalize(cross(output.ViewTangent, output.ViewNormal));  // tangent, normal 벡터 외적 시 binormal 등장

    return output;
}

float4 PSMain(VSOut input) : SV_Target
{
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);  // temp: 흰색으로 설정
    if (GUseTexture_0 == 1)
    {
        Color = GTexture_0.Sample(GSampler_0, input.UV);
    }

    float3 ViewNormal = input.ViewNormal;
	if (GUseTexture_1 == 1)
	{
        // RGB 0 ~ 255 -> 0 ~ 1 변환
        float3 TangentSpaceNormal = GTexture_1.Sample(GSampler_0, input.UV).xyz;
        // 0 ~ 1에서 -1 ~ 1 변환
		TangentSpaceNormal = (TangentSpaceNormal - 0.5f) * 2.f;

        float3x3 TBNMatrix = { input.ViewTangent, input.ViewBinormal, input.ViewNormal };
        ViewNormal = normalize(mul(TangentSpaceNormal, TBNMatrix));
    }

    LightColor TotalColor = (LightColor) 0.f;

    for (int i = 0; i < GNumLight; ++i)
    {
        LightColor Light = CalculateLightColor(i, input.ViewNormal, input.ViewPosition);
        TotalColor.Diffuse += Light.Diffuse;
        TotalColor.Ambient += Light.Ambient;
        TotalColor.Specular += Light.Specular;
    }

    Color.xyz = TotalColor.Diffuse.xyz * Color.xyz
	+ TotalColor.Ambient.xyz * Color.xyz
	+ TotalColor.Specular.xyz;  // 물체와 무관하기 때문에 그냥 더해줌

    return Color;
}

/**
 *  Texture Shader
 *  GTexture_0: Output Texture
 *  AlphaBlend: true
 */

struct VSTexIn
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct VSTexOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
};

VSTexOut VSTex(VSTexIn Input)
{
    VSTexOut Output = (VSTexOut) 0;

    Output.Position = mul(float4(Input.Position, 1.f), GWorldViewProjectionMatrix);
    Output.UV = Input.UV;

    return Output;
}

float4 PSTex(VSTexOut Input) : SV_Target
{
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
	if (GUseTexture_0)
	{
        Color = GTexture_0.Sample(GSampler_0, Input.UV);
    }

    return Color;
}

#endif