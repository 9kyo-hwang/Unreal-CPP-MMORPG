#ifndef _UTILS_FX_
#define _UTILS_FX_
#include "Parameters.fx"

// Calculate in View Coordinate
LightColor CalculateLightColor(int index, float3 normal, float3 position)
{
    LightColor color = (LightColor) 0.f;
    float3 lightDirection;

    // diffuseRatio: �Ի簢�� ���� ���� ������� �������� ��Ÿ��. cos(theta)�� ����
    // distanceRatio: Point/Spot ���� ������ ����� ���� �ƿ� �������Ƿ�, �̸� ��Ÿ���� ���� ���
    float diffuseRatio, distanceRatio = 1.f;

    if (g_light[index].lightType == 0)  // Directional
    {
		//  view ��ǥ�� �������� ����ϱ� ���� ��ȯ
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

    // ������ ��ü -> ī�޶� ���Ͱ� �ʿ��ѵ�, View ��ǥ���̹Ƿ� ī�޶� ��ǥ�� (0, 0, 0)��
	// �� normalize(position - (0, 0, 0))�� �� ī�޶� ������ �������̹Ƿ� �̸� -�ؼ� ����ϸ� ��
	float3 eyeDirection = normalize(position);
    float specularRatio = saturate(dot(-eyeDirection, reflectionDirection));
    specularRatio = pow(specularRatio, 2);  // ������ ���� �۾�(������ �� ������ �����ϱ� ����)

    color.diffuse = g_light[index].color.diffuse * diffuseRatio * diffuseRatio;
    color.ambient = g_light[index].color.ambient * distanceRatio;
    color.specular = g_light[index].color.specular * specularRatio * distanceRatio;

    return color;
}

#endif