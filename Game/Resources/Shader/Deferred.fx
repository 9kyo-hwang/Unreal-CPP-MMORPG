#ifndef _DEFERRED_FX_
#define _DEFERRED_FX_

#include "Parameters.fx"

struct VSIn
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    
    row_major matrix WorldMatrix : W;
    row_major matrix WorldViewMatrix : WV;
    row_major matrix WorldViewProjectionMatrix : WVP;
    uint InstanceID : SV_InstanceID;
};

struct VSOut
{
    float4 Position : SV_Position;   // MS���� ������ ��Ģ(System Value)
    float2 UV : TEXCOORD;
    float3 ViewPosition : POSITION;
    float3 ViewNormal : NORMAL;
    float3 ViewTangent : TANGENT;
    float3 ViewBinormal : BINORMAL;
};

VSOut VSMain(VSIn Input)
{
    VSOut Output = (VSOut)0;

    // ���� ��ǥ���� �����ϵ��� wvp_matrix�� ���������� ������
	// 1.f: ����� ���� �� ��ǥ�� �ν�, 0.f: ���⼺�� ����
	if(GInt_0 == 0)
	{
	    Output.Position = mul(float4(Input.Position, 1.f), GWorldViewProjectionMatrix); // projection ��ǥ��� �Ѿ
        Output.UV = Input.UV;
        Output.ViewPosition = mul(float4(Input.Position, 1.f), GWorldViewMatrix).xyz; // view ��ǥ�谡 �ʿ�
        Output.ViewNormal = normalize(mul(float4(Input.Normal, 0.f), GWorldViewMatrix).xyz);    // translation�� ������� �ʵ��� 0.f ����
        Output.ViewTangent = normalize(mul(float4(Input.Tangent, 0.f), GWorldViewMatrix).xyz);    // translation�� ������� �ʵ��� 0.f ����
        Output.ViewBinormal = normalize(cross(Output.ViewTangent, Output.ViewNormal));  // tangent, normal ���� ���� �� binormal ����
	}
    else
    {
        Output.Position = mul(float4(Input.Position, 1.f), Input.WorldViewProjectionMatrix); // projection ��ǥ��� �Ѿ
        Output.UV = Input.UV;
        Output.ViewPosition = mul(float4(Input.Position, 1.f), Input.WorldViewMatrix).xyz; // view ��ǥ�谡 �ʿ�
        Output.ViewNormal = normalize(mul(float4(Input.Normal, 0.f), Input.WorldViewMatrix).xyz);    // translation�� ������� �ʵ��� 0.f ����
        Output.ViewTangent = normalize(mul(float4(Input.Tangent, 0.f), Input.WorldViewMatrix).xyz);    // translation�� ������� �ʵ��� 0.f ����
        Output.ViewBinormal = normalize(cross(Output.ViewTangent, Output.ViewNormal));  // tangent, normal ���� ���� �� binormal ����
    }

    return Output;
}

struct PSOut
{
    float4 Position : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Color : SV_Target2;
};

PSOut PSMain(VSOut Input)
{
    PSOut Output = (PSOut) 0;

    float4 Color = float4(1.f, 1.f, 1.f, 1.f);  // temp: ������� ����
    if (GUseTexture_0 == 1)
    {
        Color = GTexture_0.Sample(GSampler_0, Input.UV);
    }

    float3 ViewNormal = Input.ViewNormal;
	if (GUseTexture_1 == 1)
	{
        // RGB 0 ~ 255 -> 0 ~ 1 ��ȯ
        float3 TangentSpaceNormal = GTexture_1.Sample(GSampler_0, Input.UV).xyz;
        // 0 ~ 1���� -1 ~ 1 ��ȯ
		TangentSpaceNormal = (TangentSpaceNormal - 0.5f) * 2.f;

        float3x3 TBNMatrix = { Input.ViewTangent, Input.ViewBinormal, Input.ViewNormal };
        ViewNormal = normalize(mul(TangentSpaceNormal, TBNMatrix));
    }

    Output.Position = float4(Input.ViewPosition.xyz, 0.f);  // �߰� �����͸� ����
    Output.Normal = float4(ViewNormal.xyz, 0.f);   // �߰� �����͸� ����
    Output.Color = Color;

    return Output;
}

#endif