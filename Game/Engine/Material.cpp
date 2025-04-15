#include "pch.h"
#include "Material.h"

#include "Engine.h"
#include "Shader.h"
#include "Texture.h"

FMaterial::FMaterial()
	: Super(EObjectType::Material)
{
}

FMaterial::~FMaterial()
{
}

void FMaterial::PushData()
{
	CONSTANT_BUFFER(EConstantBufferType::Material)->PushData(&Parameters, sizeof(Parameters));	// Upload CBV

	for (uint32 Index = 0; Index < Textures.size(); ++Index)	// Upload SRV
	{
		if (!Textures[Index])
		{
			continue;
		}

		EShaderResourceViewRegisters Register = static_cast<EShaderResourceViewRegisters>(static_cast<int8>(EShaderResourceViewRegisters::t0) + Index);
		GEngine->GetTableDescriptorHeap()->SetShaderResourceView(Textures[Index]->GetShaderResourceDescriptorHandle(), Register);
	}

	Shader->Update();	// Set Pipeline
}
