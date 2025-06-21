#pragma once
#include "Object.h"

class FShader;
class FTexture;

/**
 *	.hlsli 파일의 cbuffer register(b1)에서 사용하게 될 구조체
 */

constexpr uint8 NumMaterialParamArgs = 4;

struct FMaterialParameters
{
public:
	FMaterialParameters()
		: IntParameters()
		, FloatParameters()
		, UsedTextureParameters()
		, Vector2Parameters()
		, MatrixParameters()
	{}

	void Set(uint8 Index, int32 Value) { IntParameters[Index] = Value; }
	void Set(uint8 Index, float Value) { FloatParameters[Index] = Value; }
	void Set(uint8 Index, FVector2 Value) { Vector2Parameters[Index] = Value; }
	void Set(uint8 Index, FVector4 Value) { Vector4Parameters[Index] = Value; }
	void Set(uint8 Index, FMatrix Value) { MatrixParameters[Index] = Value; }
	void SetUseTexture(uint8 Index, int32 Use) { UsedTextureParameters[Index] = Use; }

private:
	array<int32, NumMaterialParamArgs> IntParameters;
	array<float, NumMaterialParamArgs> FloatParameters;
	array<int32, NumMaterialParamArgs> UsedTextureParameters;
	array<FVector2, NumMaterialParamArgs> Vector2Parameters;
	array<FVector4, NumMaterialParamArgs> Vector4Parameters;
	array<FMatrix, NumMaterialParamArgs> MatrixParameters;
};

class FMaterial : public UObject
{
	using Super = UObject;

public:
	FMaterial();
	~FMaterial() override;

	TSharedPtr<FMaterial> Clone() const;
	
	TSharedPtr<FShader> GetShader() { return Shader; }
	void SetShader(TSharedPtr<FShader> InShader) { Shader = InShader; }
	
	void SetParameter(uint8 Index, int32 Value) { Parameters.Set(Index, Value); }
	void SetParameter(uint8 Index, float Value) { Parameters.Set(Index, Value); }
	void SetParameter(uint8 Index, FVector2 Value) { Parameters.Set(Index, Value); }
	void SetParameter(uint8 Index, FVector4 Value) { Parameters.Set(Index, Value); }
	void SetParameter(uint8 Index, FMatrix Value) { Parameters.Set(Index, Value); }
	void SetTexture(uint8 Index, TSharedPtr<FTexture> Texture)
	{
		Textures[Index] = Texture;
		Parameters.SetUseTexture(Index, Texture ? 1 : 0);
	}

	void PushGraphicsData();
	void PushComputeData();
	void Dispatch(uint32 X, uint32 Y, uint32 Z);

private:
	TSharedPtr<FShader> Shader;
	FMaterialParameters Parameters;
	array<TSharedPtr<FTexture>, NumMaterialParamArgs> Textures;
};

