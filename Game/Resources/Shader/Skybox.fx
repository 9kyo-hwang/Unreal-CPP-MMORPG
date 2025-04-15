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
    float4 Position : SV_Position;   // MS에서 지정한 규칙(System Value)
    float2 UV : TEXCOORD;
};

VSOut VSMain(VSIn Input)
{
    VSOut Output = (VSOut)0;

    // translation만 넘기기 위해 w=0
    float4 ViewPosition = mul(float4(Input.LocalPosition, 0.0f), GViewMatrix);

    // view에서 projection으로 변할 때 동차좌표계(clip space) 존재
	float4 ClipSpacePosition = mul(ViewPosition, GProjectionMatrix);

    // x, y, z, w 중 z 값을 이용해 깊이를 판별. 이 값을 보존하기 위해 w 쪽으로 넘겨놨었음

    /**
     *  Rasterizer 단계에서 (x/w, y/w, z/w, w/w)로 반환됨
     *  Skybox는 항상 Z = 1 위치에 고정시키고 싶음
     *  따라서 z값을 w로 대체해주면 (x/w, y/w, w/w, w/w)가 되어 Z = 1이 고정됨
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