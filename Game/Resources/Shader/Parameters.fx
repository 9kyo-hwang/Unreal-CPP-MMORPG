#ifndef _PARAMETERS_FX_
#define _PARAMETERS_FX_

struct LightColor
{
    float4 diffuse; // 16byte 단위를 맞추기 위해 float3가 아닌 4를 사용
    float4 ambient;
    float4 specular;
};

struct LightInfo
{
    LightColor color;
    float4 position;
    float4 direction;
    int lightType;
    float range;
    float angle;
    int padding;    // lightType부터 여기까지 딱 16byte가 맞춰짐
};

cbuffer GLOBAL_PARAMETERS : register(b0)
{
    int g_light_count;
    float3 g_light_padding; // count 단독으로는 4byte밖에 안돼서 12byte를 패딩해줌
    LightInfo g_light[50];
}

cbuffer TRANSFORM_PARAMETERS : register(b1)
{
// row_major: shader <-> dx 행렬 해석 순서가 달라 일치시키기 위함
    row_major matrix g_world_matrix;
    row_major matrix g_view_matrix;
    row_major matrix g_projection_matrix;
    row_major matrix g_wv_matrix;
    row_major matrix g_wvp_matrix;
};

cbuffer MATERIAL_PARAMETERS : register(b2)
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    int g_int_4;

    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    float g_float_4;

    int g_tex_on_0; // Shader 코드엔 null check 기능이 없어서 별도의 사용유무 변수를 둠
    int g_tex_on_1;
    int g_tex_on_2;
    int g_tex_on_3;
    int g_tex_on_4;
};

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);

SamplerState g_sam_0 : register(s0);

#endif