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
 *  GInt_0: Light Index. GlobalParameters에 LightInfo Light[50]; 으로 들고 있으므로 Index를 통해 어떤 빛인지 획득
 *  GTexture_0: Position Render Target
 *  GTexture_1: Normal Render Target
 *  Mesh: Rectangle
 */

VSOut VSDirectionalLight(VSIn Input)
{
    VSOut Output = (VSOut) 0;

    /**
     *  Resources에서 Rectangle을 WidthHalf, HeightHalf 사이즈로 계산 중(투영 좌표계에서 -1~1로 표시, 중앙이 0, 0이기 때문)
     *  따라서 x2를 통해 전체 영역을 지정
     *  Directional Light는 범위 제한이 없어서 화면 전체 사이즈를 지정
     */
    Output.Position = float4(Input.Position * 2.f, 1.f);
    Output.UV = Input.UV;

    return Output;
}

PSOut PSDirectionalLight(VSOut Input)
{
	/**
	 *  Texture 0/1에 각각 Position/Normal 정보 존재 -> 물체 유무 판별
	 *  물체가 없다면 Skip(Clip, 카메라 기준 좌표계에서 카메라보다 뒤에 있는 물체를 의미)
	 *  물체가 있다면 그 때 빛 연산 수행
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
    // Input.Position은 SV_Position, 즉 여기로 넘어올 때 Screen 좌표계로 알아서 보정됨(0, 0 ~ 800, 600)
    float2 UV = float2(Input.Position.x / GVector2_0.x, Input.Position.y / GVector2_0.y);
    float3 ViewPosition = GTexture_0.Sample(GSampler_0, UV).xyz;
    if (ViewPosition.z <= 0.f)
    {
        clip(-1);
    }

    int Index = GInt_0;
    float3 LightViewPosition = mul(float4(GLight[Index].Position.xyz, 1.f), GViewMatrix).xyz;
    float Distance = length(ViewPosition - LightViewPosition); // 물체 ~ 광원 거리
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
 *  최종적으로 결과물을 종합해 화면에 그려주는 역할
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
		// 빛을 받지 않는 경우
        clip(-1);
    }

    float4 Color = GTexture_0.Sample(GSampler_0, Input.UV);
    float4 Specular = GTexture_2.Sample(GSampler_0, Input.UV);

    float4 Output = (float4) 0;
    Output = (Color * LightQuantity) + Specular;
    return Output;
}

#endif