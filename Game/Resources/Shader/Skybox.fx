#ifndef _SKYBOX_FX_
#define _SKYBOX_FX_

#include "Parameters.fx"

struct VSIn
{
    float3 LocalPosition : POSITION;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 Position : SV_Position;   // MS���� ������ ��Ģ(System Value)
    float2 UV : TEXCOORD;
};

VSOut VSMain(VSIn Input)
{
    VSOut Output = (VSOut)0;

    // translation�� �ѱ�� ���� w=0
    float4 ViewPosition = mul(float4(Input.LocalPosition, 0.0f), GViewMatrix);

    // view���� projection���� ���� �� ������ǥ��(clip space) ����
	float4 ClipSpacePosition = mul(ViewPosition, GProjectionMatrix);

    // x, y, z, w �� z ���� �̿��� ���̸� �Ǻ�. �� ���� �����ϱ� ���� w ������ �Ѱܳ�����

    /**
     *  Rasterizer �ܰ迡�� (x/w, y/w, z/w, w/w)�� ��ȯ��
     *  Skybox�� �׻� Z = 1 ��ġ�� ������Ű�� ����
     *  ���� z���� w�� ��ü���ָ� (x/w, y/w, w/w, w/w)�� �Ǿ� Z = 1�� ������
     */
    Output.Position = ClipSpacePosition.xyww;
    Output.UV = Input.UV;

    return Output;
}

float4 PSMain(VSOut Input) : SV_Target
{
    float Color = GTexture_0.Sample(GSampler_0, Input.UV);
    return Color;
}

#endif