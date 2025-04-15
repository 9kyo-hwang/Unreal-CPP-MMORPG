#ifndef _LIGHTING_FX_
#define _LIGHTING_FX_

#include "Parameters.fx"
#include "Utils.fx"

struct VSIn
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
};

struct PSOut
{
    float4 Diffuse : SV_Target0;
    float4 Specular : SV_Target1;
};

/**
 *  Directional Light
 *  GInt_0: Light Index. GlobalParameters�� LightInfo Light[50]; ���� ��� �����Ƿ� Index�� ���� � ������ ȹ��
 *  GTexture_0: Position Render Target
 *  GTexture_1: Normal Render Target
 *  Mesh: Rectangle
 */

VSOut VSDirectionalLight(VSIn Input)
{
    VSOut Output = (VSOut) 0;

    /**
     *  Resources���� Rectangle�� WidthHalf, HeightHalf ������� ��� ��(���� ��ǥ�迡�� -1~1�� ǥ��, �߾��� 0, 0�̱� ����)
     *  ���� x2�� ���� ��ü ������ ����
     *  Directional Light�� ���� ������ ��� ȭ�� ��ü ����� ����
     */
    Output.Position = float4(Input.Position * 2.f, 1.f);
    Output.UV = Input.UV;

    return Output;
}

PSOut PSDirectionalLight(VSOut Input)
{
	/**
	 *  Texture 0/1�� ���� Position/Normal ���� ���� -> ��ü ���� �Ǻ�
	 *  ��ü�� ���ٸ� Skip(Clip, ī�޶� ���� ��ǥ�迡�� ī�޶󺸴� �ڿ� �ִ� ��ü�� �ǹ�)
	 *  ��ü�� �ִٸ� �� �� �� ���� ����
	 */
    float3 ViewPosition = GTexture_0.Sample(GSampler_0, Input.UV).xyz;
	if (ViewPosition.z <= 0.f)
	{
        clip(-1);
    }

    float3 ViewNormal = GTexture_1.Sample(GSampler_0, Input.UV).xyz;
    LightColor Color = CalculateLightColor(GInt_0, ViewNormal, ViewPosition);

    PSOut Output = (PSOut) 0;
    Output.Diffuse = Color.Diffuse + Color.Ambient;
    Output.Specular = Color.Specular;

    return Output;
}

/**
 *  Point Light
 *  GInt_0: Index
 *  GTexture_0: Position Render Target
 *  GTexture_1: Normal Render Target
 *  GVector2_0: Render Target Resolution(800 x 600)
 *  Mesh: Sphere
 */

VSOut VSPointLight(VSIn Input)
{
    VSOut Output = (VSOut) 0;
    Output.Position = mul(float4(Input.Position, 1.f), GWorldViewProjectionMatrix);
    Output.UV = Input.UV;

    return Output;
}

PSOut PSPointLight(VSOut Input)
{
    // Input.Position�� SV_Position, �� ����� �Ѿ�� �� Screen ��ǥ��� �˾Ƽ� ������(0, 0 ~ 800, 600)
    float2 UV = float2(Input.Position.x / GVector2_0.x, Input.Position.y / GVector2_0.y);
    float3 ViewPosition = GTexture_0.Sample(GSampler_0, UV).xyz;
    if (ViewPosition.z <= 0.f)
    {
        clip(-1);
    }

    int Index = GInt_0;
    float3 LightViewPosition = mul(float4(GLight[Index].Position.xyz, 1.f), GViewMatrix).xyz;
    float Distance = length(ViewPosition - LightViewPosition); // ��ü ~ ���� �Ÿ�
	if (Distance > GLight[Index].Range)
	{
        clip(-1);
    }

    float3 ViewNormal = GTexture_1.Sample(GSampler_0, UV).xyz;
    LightColor Color = CalculateLightColor(GInt_0, ViewNormal, ViewPosition);

    PSOut Output = (PSOut) 0;
    Output.Diffuse = Color.Diffuse + Color.Ambient;
    Output.Specular = Color.Specular;

    return Output;
}

/**
 *  ���������� ������� ������ ȭ�鿡 �׷��ִ� ����
 *  GTexture_0: Diffuse/Color Target
 *  GTexture_1: Diffuse Light Target
 *  GTexture_2: Specular Light Target
 *  Mesh: Rectangle
 */
VSOut VSFinal(VSIn Input)
{
    VSOut Output = (VSOut) 0;

    Output.Position = float4(Input.Position * 2.f, 1.f);
    Output.UV = Input.UV;

    return Output;
}

float4 PSFinal(VSOut Input) : SV_Target
{
    float4 LightQuantity = GTexture_1.Sample(GSampler_0, Input.UV);
	if (LightQuantity.x == 0.f && LightQuantity.y == 0.f && LightQuantity.z == 0.f)
	{
		// ���� ���� �ʴ� ���
        clip(-1);
    }

    float4 Color = GTexture_0.Sample(GSampler_0, Input.UV);
    float4 Specular = GTexture_2.Sample(GSampler_0, Input.UV);

    float4 Output = (float4) 0;
    Output = (Color * LightQuantity) + Specular;
    return Output;
}

#endif