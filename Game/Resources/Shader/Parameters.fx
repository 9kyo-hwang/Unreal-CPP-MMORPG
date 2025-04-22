#ifndef _PARAMETERS_FX_
#define _PARAMETERS_FX_

struct LightColor
{
    float4 Diffuse; // 16byte 단위를 맞추기 위해 float3가 아닌 4를 사용
    float4 Ambient;
    float4 Specular;
};

struct LightInfo
{
    LightColor Color;
    float4 Position;
    float4 Direction;
    int LightType;
    float Range;
    float Angle;
    int Padding;    // lightType부터 여기까지 딱 16byte가 맞춰짐
};

cbuffer GLOBAL_PARAMETERS : register(b0)
{
    int GNumLight;
    float3 GLightPadding; // count 단독으로는 4byte밖에 안돼서 12byte를 패딩해줌
    LightInfo GLight[50];
}

cbuffer TRANSFORM_PARAMETERS : register(b1)
{
// row_major: shader <-> dx 행렬 해석 순서가 달라 일치시키기 위함
    row_major matrix GWorldMatrix;
    row_major matrix GViewMatrix;
    row_major matrix GProjectionMatrix;
    row_major matrix GWorldViewMatrix;
    row_major matrix GWorldViewProjectionMatrix;
    row_major matrix GInverseViewMatrix;
};

cbuffer MATERIAL_PARAMETERS : register(b2)
{
	// 16byte를 맞추기 위해 4개씩
    int GInt_0;
    int GInt_1;
    int GInt_2;
    int GInt_3;

    float GFloat_0;
    float GFloat_1;
    float GFloat_2;
    float GFloat_3;

    int GUseTexture_0; // Shader 코드엔 null check 기능이 없어서 별도의 사용유무 변수를 둠
    int GUseTexture_1;
    int GUseTexture_2;
    int GUseTexture_3;

    float2 GVector2_0;
    float2 GVector2_1;
    float2 GVector2_2;
    float2 GVector2_3;

    float4 GVector4_0;
    float4 GVector4_1;
    float4 GVector4_2;
    float4 GVector4_3;

    row_major float4x4 GMatrix_0;
    row_major float4x4 GMatrix_1;
    row_major float4x4 GMatrix_2;
    row_major float4x4 GMatrix_3;
};

Texture2D GTexture_0 : register(t0);
Texture2D GTexture_1 : register(t1);
Texture2D GTexture_2 : register(t2);
Texture2D GTexture_3 : register(t3);
Texture2D GTexture_4 : register(t4);

SamplerState GSampler_0 : register(s0);

#endif