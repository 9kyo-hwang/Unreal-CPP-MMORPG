#ifndef _SKYBOX_HLSLI_
#define _SKYBOX_HLSLI_

#include "Parameters.hlsli"

struct VS_IN
{
    float3 local_pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;   // MS���� ������ ��Ģ(System Value)
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // translation�� �ѱ�� ���� w=0
    float4 view_pos = mul(float4(input.local_pos, 0.0f), g_view_matrix);

    // view���� projection���� ���� �� ������ǥ��(clip space) ����
	float4 clip_space_pos = mul(view_pos, g_projection_matrix);

    // x, y, z, w �� z ���� �̿��� ���̸� �Ǻ�. �� ���� �����ϱ� ���� w ������ �Ѱܳ�����

    /**
     *  Rasterizer �ܰ迡�� (x/w, y/w, z/w, w/w)�� ��ȯ��
     *  Skybox�� �׻� Z = 1 ��ġ�� ������Ű�� ����
     *  ���� z���� w�� ��ü���ָ� (x/w, y/w, w/w, w/w)�� �Ǿ� Z = 1�� ������
     */
    output.pos = clip_space_pos.xyww;
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float color = g_tex_0.Sample(g_sam_0, input.uv);
    return color;
}

#endif