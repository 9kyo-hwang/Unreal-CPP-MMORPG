#pragma once
#include "Object.h"

class FShader;
class FTexture;

/**
 *	.hlsli 파일의 cbuffer register(b1)에서 사용하게 될 구조체
 */

constexpr uint8 MaterialIntCount = 5;
constexpr uint8 MaterialFloatCount = 5;
constexpr uint8 MaterialTextureCount = 5;

struct FMaterialParameters
{
	void Set(uint8 Index, int32 Value) { IntParameters[Index] = Value; }
	void Set(uint8 Index, float Value) { FloatParameters[Index] = Value; }
	void SetUseTexture(uint8 Index, int32 Use) { UsedTextureParameters[Index] = Use; }

	array<int32, MaterialIntCount> IntParameters;
	array<float, MaterialFloatCount> FloatParameters;
	array<int32, MaterialTextureCount> UsedTextureParameters;
};

class FMaterial : public Object
{
	using Super = Object;

public:
	FMaterial();
	~FMaterial() override;

	shared_ptr<FShader> GetShader() { return Shader; }

	void SetShader(shared_ptr<FShader> InShader) { Shader = InShader; }
	void SetMaterialParameters(uint8 Index, int32 Value) { MaterialParameters.Set(Index, Value); }
	void SetMaterialParameters(uint8 Index, float Value) { MaterialParameters.Set(Index, Value); }
	void SetTexture(uint8 Index, shared_ptr<FTexture> Texture)
	{
		Textures[Index] = Texture;
		MaterialParameters.SetUseTexture(Index, Texture ? 1 : 0);
	}

	void PushData();

private:
	shared_ptr<FShader> Shader;
	FMaterialParameters MaterialParameters{};
	array<shared_ptr<FTexture>, MaterialTextureCount> Textures;
};

