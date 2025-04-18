#pragma once
#include "Object.h"

class FShader;
class FTexture;

/**
 *	.hlsli 파일의 cbuffer register(b1)에서 사용하게 될 구조체
 */

constexpr uint8 NumMaterialInt = 4;
constexpr uint8 NumMaterialFloat = 4;
constexpr uint8 NumMaterialTexture = 4;
constexpr uint8 NumMaterialVector2 = 4;
constexpr uint8 NumMaterialVector4 = 4;

struct FMaterialParameters
{
	void Set(uint8 Index, int32 Value) { IntParameters[Index] = Value; }
	void Set(uint8 Index, float Value) { FloatParameters[Index] = Value; }
	void Set(uint8 Index, FVector2 Value) { Vector2Parameters[Index] = Value; }
	void Set(uint8 Index, FVector4 Value) { Vector4Parameters[Index] = Value; }
	void SetUseTexture(uint8 Index, int32 Use) { UsedTextureParameters[Index] = Use; }

	array<int32, NumMaterialInt> IntParameters;
	array<float, NumMaterialFloat> FloatParameters;
	array<int32, NumMaterialTexture> UsedTextureParameters;
	array<FVector2, NumMaterialVector2> Vector2Parameters;
	array<FVector4, NumMaterialVector4> Vector4Parameters;
};

class FMaterial : public Object
{
	using Super = Object;

public:
	FMaterial();
	~FMaterial() override;

	shared_ptr<FShader> GetShader() { return Shader; }

	void SetShader(shared_ptr<FShader> InShader) { Shader = InShader; }
	void SetParameter(uint8 Index, int32 Value) { Parameters.Set(Index, Value); }
	void SetParameter(uint8 Index, float Value) { Parameters.Set(Index, Value); }
	void SetParameter(uint8 Index, FVector2 Value) { Parameters.Set(Index, Value); }
	void SetParameter(uint8 Index,FVector4 Value) { Parameters.Set(Index, Value); }
	void SetTexture(uint8 Index, shared_ptr<FTexture> Texture)
	{
		Textures[Index] = Texture;
		Parameters.SetUseTexture(Index, Texture ? 1 : 0);
	}

	void PushGraphicsData();
	void PushComputeData();
	void Dispatch(uint32 X, uint32 Y, uint32 Z);

private:
	shared_ptr<FShader> Shader;
	FMaterialParameters Parameters;
	array<shared_ptr<FTexture>, NumMaterialTexture> Textures;
};

