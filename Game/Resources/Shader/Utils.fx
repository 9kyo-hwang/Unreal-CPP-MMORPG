#ifndef _UTILS_FX_
#define _UTILS_FX_
#include "Parameters.fx"

// Calculate in View Coordinate
LightColor CalculateLightColor(int index, float3 normal, float3 position)
{
    LightColor color = (LightColor) 0.f;
    float3 lightDirection;

    // diffuseRatio: 입사각에 따른 빛의 영향력이 감소함을 나타냄. cos(theta)의 역할
    // distanceRatio: Point/Spot 빛은 범위를 벗어나면 빛을 아예 못받으므로, 이를 나타내기 위해 사용
    float diffuseRatio, distanceRatio = 1.f;

    if (g_light[index].lightType == 0)  // Directional
    {
		//  view 좌표계 기준으로 계산하기 위해 변환
        lightDirection = normalize(mul(float4(g_light[index].direction.xyz, 0.f), g_view_matrix).xyz);
        diffuseRatio = saturate(dot(-lightDirection, normal));  // |N||L|cos(theta)
    }
	else if (g_light[index].lightType == 1)  // Point
	{
        float3 lightPosition = mul(float4(g_light[index].position.xyz, 1.f), g_view_matrix).xyz;
        lightDirection = normalize(position - lightPosition);
        diffuseRatio = saturate(dot(-lightDirection, normal));

        float dist = distance(position, lightPosition);
        if (g_light[index].range == 0.f)
        {
            distanceRatio = 0.f;
        }
		else
		{
            distanceRatio = saturate(1.f - pow(dist / g_light[index].range, 2));
        }
    }
	else  // Spot
	{
        float3 lightPosition = mul(float4(g_light[index].position.xyz, 1.f), g_view_matrix).xyz;
        lightDirection = normalize(position - lightPosition);
        diffuseRatio = saturate(dot(-lightDirection, normal));

        if (g_light[index].range == 0.f)
        {
            distanceRatio = 0.f;
        }
		else
		{
            float halfAngle = g_light[index].angle / 2;
            float3 lightVector = position - lightPosition;
            float3 centerLightDirection = normalize(mul(float4(g_light[index].direction.xyz, 0.f), g_view_matrix).xyz);

            float centerDistance = dot(lightVector, centerLightDirection);
            distanceRatio = saturate(1.f - centerDistance / g_light[index].range);

            float lightAngle = acos(dot(normalize(lightVector), centerLightDirection));

            bool outOfRange = centerDistance < 0.f || centerDistance > g_light[index].range;
            bool outOfAngle = lightAngle > halfAngle;
            distanceRatio = outOfRange || outOfAngle
			? 0.f
			: saturate(1.f - pow(centerDistance / g_light[index].range, 2));
        }
    }

    float3 reflectionDirection = normalize(lightDirection + 2 * (saturate(dot(-lightDirection, normal)) * normal));

    // 원래는 물체 -> 카메라 벡터가 필요한데, View 좌표계이므로 카메라 좌표가 (0, 0, 0)임
	// 즉 normalize(position - (0, 0, 0))가 곧 카메라 벡터의 역방향이므로 이를 -해서 사용하면 됨
	float3 eyeDirection = normalize(position);
    float specularRatio = saturate(dot(-eyeDirection, reflectionDirection));
    specularRatio = pow(specularRatio, 2);  // 일종의 보정 작업(영역을 더 엄격히 제한하기 위해)

    color.diffuse = g_light[index].color.diffuse * diffuseRatio * diffuseRatio;
    color.ambient = g_light[index].color.ambient * distanceRatio;
    color.specular = g_light[index].color.specular * specularRatio * distanceRatio;

    return color;
}

#endif