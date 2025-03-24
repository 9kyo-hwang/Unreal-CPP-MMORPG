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
    float4 pos : SV_Position;   // MS���� ������ ��Ģ(System Value)
    float2 uv : TEXCOORD;
    float3 view_pos : POSITION;
    float3 view_normal : NORMAL;
    float3 view_tangent : TANGENT;
    float3 view_binormal : BINORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // ���� ��ǥ���� �����ϵ��� wvp_matrix�� ���������� ������
	// 1.f: ����� ���� �� ��ǥ�� �ν�, 0.f: ���⼺�� ����
    output.pos = mul(float4(input.pos, 1.f), g_wvp_matrix); // projection ��ǥ��� �Ѿ
    output.uv = input.uv;
    output.view_pos = mul(float4(input.pos, 1.f), g_wv_matrix).xyz; // view ��ǥ�谡 �ʿ�
    output.view_normal = normalize(mul(float4(input.normal, 0.f), g_wv_matrix).xyz);    // translation�� ������� �ʵ��� 0.f ����
    output.view_tangent = normalize(mul(float4(input.tangent, 0.f), g_wv_matrix).xyz);    // translation�� ������� �ʵ��� 0.f ����
    output.view_binormal = normalize(cross(output.view_tangent, output.view_normal));  // tangent, normal ���� ���� �� binormal ����

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);  // temp: ������� ����
    if (g_tex_on_0 == 1)
    {
        color = g_tex_0.Sample(g_sam_0, input.uv);
    }

    float3 view_normal = input.view_normal;
	if (g_tex_on_1 == 1)
	{
        // RGB 0 ~ 255 -> 0 ~ 1 ��ȯ
        float3 tangent_space_normal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // 0 ~ 1���� -1 ~ 1 ��ȯ
		tangent_space_normal = (tangent_space_normal - 0.5f) * 2.f;

        float3x3 tbn_matrix = { input.view_tangent, input.view_binormal, input.view_normal };
        view_normal = normalize(mul(tangent_space_normal, tbn_matrix));
    }

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
	+ totalColor.specular.xyz;  // ��ü�� �����ϱ� ������ �׳� ������

    return color;
}

#endif