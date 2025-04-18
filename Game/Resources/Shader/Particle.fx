#ifndef _PARTICLE_FX_
#define _PARTICLE_FX_

#include "Parameters.fx"
#include "Utils.fx"

struct Particle
{
    float3 WorldPosition;
    float ElapsedTime;
    float3 WorldDirection;
    float Time;
    int bAlive;
    float3 Padding;
};

StructuredBuffer<Particle> GData : register(t9);    // 아래 CSMain의 결과가 t9 레지스터에 저장

struct VSIn
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    uint ID : SV_InstanceID;
};

struct VSOut
{
    float4 ViewPosition : POSITION;
    float2 UV : TEXCOORD;
    float ID : ID;
};

/**
 *  [VSMain]
 *  GFloat_0: Start Scale
 *  GFloat_1: End Scale
 *  GTexture_0: Particle Texture
 */
VSOut VSMain(VSIn Input)
{
    float3 WorldPosition = mul(float4(Input.Position, 1.f), GWorldMatrix).xyz;
    WorldPosition += GData[Input.ID].WorldPosition;

    VSOut Output = (VSOut) 0.f;
    Output.ViewPosition = mul(float4(WorldPosition, 1.f), GViewMatrix);
    Output.UV = Input.UV;
    Output.ID = Input.ID;

    return Output;
}

struct GSOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
    uint ID : SV_InstanceID;
};

// GS 단계를 컨트롤해야 해서 정점 정보 리스트를 넘겨주는 것이 아닌, 점 1개에 대한 정보만 넘겨줌
[maxvertexcount(6)]
void GSMain(point VSOut Input[1], inout TriangleStream<GSOut> StreamOutput)
{
    VSOut Vertex = Input[0];
    uint ID = (uint) Vertex.ID;
	if (0 == GData[ID].bAlive)
	{
        return;
    }

    float Ratio = GData[ID].ElapsedTime / GData[ID].Time;
    float Scale = ((GFloat_1 - GFloat_0) * Ratio + GFloat_0) / 2.f;
    GSOut Output[4] = { (GSOut) 0.f, (GSOut) 0.f, (GSOut) 0.f, (GSOut) 0.f };

    // View Space
    Output[0].Position = Vertex.ViewPosition + float4(-Scale, Scale, 0.f, 0.f);
    Output[1].Position = Vertex.ViewPosition + float4(Scale, Scale, 0.f, 0.f);
    Output[2].Position = Vertex.ViewPosition + float4(Scale, -Scale, 0.f, 0.f);
    Output[3].Position = Vertex.ViewPosition + float4(-Scale, -Scale, 0.f, 0.f);

    // Projection Space
    Output[0].Position = mul(Output[0].Position, GProjectionMatrix);
    Output[1].Position = mul(Output[1].Position, GProjectionMatrix);
    Output[2].Position = mul(Output[2].Position, GProjectionMatrix);
    Output[3].Position = mul(Output[3].Position, GProjectionMatrix);

    Output[0].UV = float2(0.f, 0.f);
    Output[1].UV = float2(1.f, 0.f);
    Output[2].UV = float2(1.f, 1.f);
    Output[3].UV = float2(0.f, 1.f);

    Output[0].ID = ID;
    Output[1].ID = ID;
    Output[2].ID = ID;
    Output[3].ID = ID;

    StreamOutput.Append(Output[0]);
    StreamOutput.Append(Output[1]);
    StreamOutput.Append(Output[2]);
    StreamOutput.RestartStrip();

    StreamOutput.Append(Output[0]);
    StreamOutput.Append(Output[2]);
    StreamOutput.Append(Output[3]);
    StreamOutput.RestartStrip();
}

float4 PSMain(GSOut Input) : SV_Target
{
    return GTexture_0.Sample(GSampler_0, Input.UV);
}

struct SharedComputeBuffer  // 크기가 1인 배열
{
    int NumCanSpawn;
    float3 Padding;
};

RWStructuredBuffer<Particle> GParticle : register(u0);
RWStructuredBuffer<SharedComputeBuffer> GSharedBuffer : register(u1);   

/**
 *  [CSMain]
 *  GVector2_1: DeltaTime, AccumulateTime
 *  GInt_0: NumMaxParticle
 *  GInt_1: NumCanSpawn
 *  GVector4_0: MinTime / MaxTime / MinSpeed / MaxSpeed
 */
[numthreads(256, 1, 1)]
void CSMain(int3 ThreadIndex : SV_DispatchThreadID)
{
	// ThreadIndex.x: 고유한 데이터
	if (ThreadIndex.x >= GInt_0)
	{
        return;
    }

    int NumMax = GInt_0;
    int NumCanSpawn = GInt_1;
    int Frame = GInt_2;
    float DeltaTime = GVector2_1.x;
    float AccumulateTime = GVector2_1.y;
    float MinTime = GVector4_0.x;
    float MaxTime = GVector4_0.y;
    float MinSpeed = GVector4_0.z;
    float MaxSpeed = GVector4_0.w;

    // 멀티쓰레드 환경이라 Thread 간 순서가 보장되지 않음을 주의, Barrier
    GSharedBuffer[0].NumCanSpawn = NumCanSpawn;
    GroupMemoryBarrierWithGroupSync();

    if (0 == GParticle[ThreadIndex.x].bAlive)
    {
		// Race
        while (true)
        {
            int NumRemainCanSpawn = GSharedBuffer[0].NumCanSpawn;
            if (NumRemainCanSpawn <= 0)
            {
                break;
            }
        
            int NumExpected = NumRemainCanSpawn;
            int NumDesired = NumRemainCanSpawn - 1;
            int OriginalValue;

	        // Dest == CompareValue라면 Value를 Dest에 대입
			// 위 결과와는 무관하게 원래 Dest 값을 OriginalValue에 대입
            InterlockedCompareExchange(GSharedBuffer[0].NumCanSpawn, NumExpected, NumDesired, OriginalValue);

            if (OriginalValue == NumExpected)
            {
                GParticle[ThreadIndex.x].bAlive = 1;
                break;
            }
        }

        if (1 == GParticle[ThreadIndex.x].bAlive)
        {
            float x = ((float) ThreadIndex.x / (float) NumMax) + AccumulateTime;

			// 임시로 Rand 함수를 정의해서 사용, 노이즈 이미지를 가져와서 랜덤값을 추출해도 OK
            float r1 = Rand(float2(x, AccumulateTime));
            float r2 = Rand(float2(x * AccumulateTime, AccumulateTime));
            float r3 = Rand(float2(x * AccumulateTime * AccumulateTime, AccumulateTime * AccumulateTime));

            float3 Noise = { 2 * r1 - 1, 2 * r2 - 1, 2 * r3 - 1 };
            float3 Direction = (Noise - 0.5f) * 2.f; // [0 - 1] -> [-1 - 1]

            GParticle[ThreadIndex.x].WorldDirection = normalize(Direction);
            GParticle[ThreadIndex.x].WorldPosition = (Noise.xyz - 0.5f) * 25;
            GParticle[ThreadIndex.x].Time = ((MaxTime - MinTime) * Noise.x) + MinTime;
            GParticle[ThreadIndex.x].ElapsedTime = 0.f;
        }
    }
	else
	{
        GParticle[ThreadIndex.x].ElapsedTime += DeltaTime;
        if (GParticle[ThreadIndex.x].ElapsedTime > GParticle[ThreadIndex.x].Time)
        {
            GParticle[ThreadIndex.x].bAlive = 0;
            return;
        }

        float Ratio = GParticle[ThreadIndex.x].ElapsedTime / GParticle[ThreadIndex.x].Time;
        float Speed = (MaxSpeed - MinSpeed) * Ratio + MinSpeed;
        GParticle[ThreadIndex.x].WorldPosition += GParticle[ThreadIndex.x].WorldDirection * Speed * DeltaTime;
    }
}

#endif