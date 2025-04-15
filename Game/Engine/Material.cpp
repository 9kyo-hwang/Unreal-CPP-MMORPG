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

void FMaterial::PushGraphicsData()
{
	CONSTANT_BUFFER(EConstantBufferType::Material)->PushGraphicsData(&Parameters, sizeof(Parameters));	// Upload CBV

	for (uint32 Index = 0; Index < Textures.size(); ++Index)	// Upload SRV
	{
		if (Textures[Index])
		{
			EShaderResourceViewRegisters Register = static_cast< EShaderResourceViewRegisters >( static_cast< int8 >( EShaderResourceViewRegisters::t0 ) + Index );
			GEngine->GetGraphicsDescriptorTable()->SetShaderResourceView(Textures[Index]->GetShaderResourceDescriptorHandle(), Register);
		}
	}

	Shader->Update();	// Set Pipeline
}

void FMaterial::PushComputeData()
{
	// CBV Upload
	CONSTANT_BUFFER(EConstantBufferType::Material)->PushComputeData(&Parameters, sizeof(Parameters));

	for (uint32 Index = 0; Index < Textures.size(); ++Index)
	{
		if (Textures[Index])
		{
			EShaderResourceViewRegisters Register = static_cast<EShaderResourceViewRegisters>(static_cast<uint8>(EShaderResourceViewRegisters::t0) + Index);
			GEngine->GetComputeDescriptorTable()->SetDescriptor(Textures[Index]->GetShaderResourceDescriptorHandle(), Register);
		}
	}

	Shader->Update();
}

void FMaterial::Dispatch(uint32 X, uint32 Y, uint32 Z)
{
	PushComputeData();	// CBV + SRV + SetPipelineState

	GEngine->GetComputeDescriptorTable()->Commit();
	COMPUTE_COMMAND_LIST->Dispatch(X, Y, Z);
	GEngine->GetComputeCommandQueue()->Flush();
}
