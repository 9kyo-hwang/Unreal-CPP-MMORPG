#ifndef _DEFAULT_HLSLI_
#define _DEFAULT_HLSLI_

#include "Parameters.hlsli"
#include "Utils.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUT
{
    float4 pos : SV_Position;   // MS에서 지정한 규칙(System Value)
    float2 uv : TEXCOORD;
    float3 view_pos : POSITION;
    float3 view_normal : NORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // 계산된 좌표값을 적용하도록 wvp_matrix를 최종적으로 곱해줌
	// 1.f: 행렬을 곱할 때 좌표로 인식, 0.f: 방향성만 추출
    output.pos = mul(float4(input.pos, 1.f), g_wvp_matrix); // projection 좌표계로 넘어감
    output.uv = input.uv;
    output.view_pos = mul(float4(input.pos, 1.f), g_wv_matrix).xyz; // view 좌표계가 필요
    output.view_normal = normalize(mul(float4(input.normal, 0.f), g_wv_matrix).xyz);    // translation이 적용되지 않도록 0.f 설정

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    //float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    float4 color = float4(1.f, 1.f, 1.f, 1.f);  // temp: 흰색으로 설정
    LightColor totalColor = (LightColor) 0.f;

    for (int i = 0; i < g_light_count; ++i)
    {
        LightColor lightColor = CalculateLightColor(i, input.view_normal, input.view_pos);
        totalColor.diffuse += lightColor.diffuse;
        totalColor.ambient += lightColor.ambient;
        totalColor.specular += lightColor.specular;
    }

    color.xyz = totalColor.diffuse.xyz * color.xyz
	+ totalColor.ambient.xyz * color.xyz
	+ totalColor.specular.xyz;  // 물체와 무관하기 때문에 그냥 더해줌

    return color;
}

#endif