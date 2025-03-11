cbuffer TRANSFORM_PARAMETERS : register(b0)
{
    row_major matrix wvp_matrix;    // row_major: shader <-> dx 행렬 해석 순서가 달라 일치시키기 위함
};

cbuffer MATERIAL_PARAMETERS : register(b1)
{
    int int_0;
    int int_1;
    int int_2;
    int int_3;
    int int_4;

    float float_0;
    float float_1;
    float float_2;
    float float_3;
    float float_4;
};

Texture2D tex_0 : register(t0);
Texture2D tex_1 : register(t1);
Texture2D tex_2 : register(t2);
Texture2D tex_3 : register(t3);
Texture2D tex_4 : register(t4);

SamplerState sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // 계산된 좌표값을 적용하도록 wvp_matrix를 최종적으로 곱해줌
	// 1.f: 행렬을 곱할 때 좌표로 인식, 0.f: 방향성만 추출
    output.pos = mul(float4(input.pos, 1.f), wvp_matrix);
	output.color = input.color;
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex_0.Sample(sam_0, input.uv);
    return color;
}