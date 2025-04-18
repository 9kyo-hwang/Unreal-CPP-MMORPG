#ifndef _UTILS_FX_
#define _UTILS_FX_
#include "Parameters.fx"

// Calculate in View Coordinate
LightColor CalculateLightColor(int Index, float3 Normal, float3 Position)
{
    LightColor Color = (LightColor) 0.f;
    float3 LightDirection;

    // diffuseRatio: �Ի簢�� ���� ���� ������� �������� ��Ÿ��. cos(theta)�� ����
    // distanceRatio: Point/Spot ���� ������ ����� ���� �ƿ� �������Ƿ�, �̸� ��Ÿ���� ���� ���
    float DiffuseRatio, DistanceRatio = 1.f;

    if (GLight[Index].LightType == 0)  // Directional
    {
		//  view ��ǥ�� �������� ����ϱ� ���� ��ȯ
        LightDirection = normalize(mul(float4(GLight[Index].Direction.xyz, 0.f), GViewMatrix).xyz);
        DiffuseRatio = saturate(dot(-LightDirection, Normal));  // |N||L|cos(theta)
    }
	else if (GLight[Index].LightType == 1)  // Point
	{
        float3 LightPosition = mul(float4(GLight[Index].Position.xyz, 1.f), GViewMatrix).xyz;
        LightDirection = normalize(Position - LightPosition);
        DiffuseRatio = saturate(dot(-LightDirection, Normal));

        float Distance = distance(Position, LightPosition);
        if (GLight[Index].Range == 0.f)
        {
            DistanceRatio = 0.f;
        }
		else
		{
            DistanceRatio = saturate(1.f - pow(Distance / GLight[Index].Range, 2));
        }
    }
	else  // Spot
	{
        float3 LightPosition = mul(float4(GLight[Index].Position.xyz, 1.f), GViewMatrix).xyz;
        LightDirection = normalize(Position - LightPosition);
        DiffuseRatio = saturate(dot(-LightDirection, Normal));

        if (GLight[Index].Range == 0.f)
        {
            DistanceRatio = 0.f;
        }
		else
		{
            float HalfAngle = GLight[Index].Angle / 2;
            float3 LightVector = Position - LightPosition;
            float3 CenterLightDirection = normalize(mul(float4(GLight[Index].Direction.xyz, 0.f), GViewMatrix).xyz);

            float CenterDistance = dot(LightVector, CenterLightDirection);
            DistanceRatio = saturate(1.f - CenterDistance / GLight[Index].Range);

            float LightAngle = acos(dot(normalize(LightVector), CenterLightDirection));

            bool OutOfRange = CenterDistance < 0.f || CenterDistance > GLight[Index].Range;
            bool OutOfAngle = LightAngle > HalfAngle;
            DistanceRatio = OutOfRange || OutOfAngle
			? 0.f
			: saturate(1.f - pow(CenterDistance / GLight[Index].Range, 2));
        }
    }

    float3 ReflectionDirection = normalize(LightDirection + 2 * (saturate(dot(-LightDirection, Normal)) * Normal));

    // ������ ��ü -> ī�޶� ���Ͱ� �ʿ��ѵ�, View ��ǥ���̹Ƿ� ī�޶� ��ǥ�� (0, 0, 0)��
	// �� normalize(position - (0, 0, 0))�� �� ī�޶� ������ �������̹Ƿ� �̸� -�ؼ� ����ϸ� ��
	float3 EyeDirection = normalize(Position);
    float SpecularRatio = saturate(dot(-EyeDirection, ReflectionDirection));
    SpecularRatio = pow(SpecularRatio, 2);  // ������ ���� �۾�(������ �� ������ �����ϱ� ����)

    Color.Diffuse = GLight[Index].Color.Diffuse * DiffuseRatio * DiffuseRatio;
    Color.Ambient = GLight[Index].Color.Ambient * DistanceRatio;
    Color.Specular = GLight[Index].Color.Specular * SpecularRatio * DistanceRatio;

    return Color;
}

float Rand(float2 UV)
{
    return frac(sin(dot(UV, float2(12.9898, 78.233))) * 43758.5453);
}

#endif