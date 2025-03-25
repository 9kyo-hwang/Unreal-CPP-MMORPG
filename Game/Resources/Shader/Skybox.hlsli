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
    float4 pos : SV_Position;   // MS에서 지정한 규칙(System Value)
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // translation만 넘기기 위해 w=0
    float4 view_pos = mul(float4(input.local_pos, 0.0f), g_view_matrix);

    // view에서 projection으로 변할 때 동차좌표계(clip space) 존재
	float4 clip_space_pos = mul(view_pos, g_projection_matrix);

    // x, y, z, w 중 z 값을 이용해 깊이를 판별. 이 값을 보존하기 위해 w 쪽으로 넘겨놨었음

    /**
     *  Rasterizer 단계에서 (x/w, y/w, z/w, w/w)로 반환됨
     *  Skybox는 항상 Z = 1 위치에 고정시키고 싶음
     *  따라서 z값을 w로 대체해주면 (x/w, y/w, w/w, w/w)가 되어 Z = 1이 고정됨
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