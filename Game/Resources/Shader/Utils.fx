#ifndef _UTILS_FX_
#define _UTILS_FX_
#include "Parameters.fx"

// Calculate in View Coordinate
LightColor CalculateLightColor(int Index, float3 Normal, float3 Position)
{
    LightColor Color = (LightColor) 0.f;
    float3 LightDirection;

    // diffuseRatio: 입사각에 따른 빛의 영향력이 감소함을 나타냄. cos(theta)의 역할
    // distanceRatio: Point/Spot 빛은 범위를 벗어나면 빛을 아예 못받으므로, 이를 나타내기 위해 사용
    float DiffuseRatio, DistanceRatio = 1.f;

    if (GLight[Index].LightType == 0)  // Directional
    {
		//  view 좌표계 기준으로 계산하기 위해 변환
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

    // 원래는 물체 -> 카메라 벡터가 필요한데, View 좌표계이므로 카메라 좌표가 (0, 0, 0)임
	// 즉 normalize(position - (0, 0, 0))가 곧 카메라 벡터의 역방향이므로 이를 -해서 사용하면 됨
	float3 EyeDirection = normalize(Position);
    float SpecularRatio = saturate(dot(-EyeDirection, ReflectionDirection));
    SpecularRatio = pow(SpecularRatio, 2);  // 일종의 보정 작업(영역을 더 엄격히 제한하기 위해)

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