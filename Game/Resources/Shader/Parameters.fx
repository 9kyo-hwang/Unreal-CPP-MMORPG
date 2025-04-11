#ifndef _PARAMETERS_FX_
#define _PARAMETERS_FX_

struct LightColor
{
    float4 diffuse; // 16byte ������ ���߱� ���� float3�� �ƴ� 4�� ���
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
    int padding;    // lightType���� ������� �� 16byte�� ������
};

cbuffer GLOBAL_PARAMETERS : register(b0)
{
    int g_light_count;
    float3 g_light_padding; // count �ܵ����δ� 4byte�ۿ� �ȵż� 12byte�� �е�����
    LightInfo g_light[50];
}

cbuffer TRANSFORM_PARAMETERS : register(b1)
{
// row_major: shader <-> dx ��� �ؼ� ������ �޶� ��ġ��Ű�� ����
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

    int g_tex_on_0; // Shader �ڵ忣 null check ����� ��� ������ ������� ������ ��
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