#include "pch.h"
#include "Material.h"

#include "Engine.h"
#include "Shader.h"
#include "Texture.h"

FMaterial::FMaterial()
	: Super(EObjectType::Material)
	, Parameters()
{
}

FMaterial::~FMaterial() = default;

TSharedPtr<FMaterial> FMaterial::Clone() const
{
	TSharedPtr<FMaterial> Material = MakeShared<FMaterial>();

	Material->SetShader(Shader);
	Material->Parameters = Parameters;
	Material->Textures = Textures;

	return Material;
}

void FMaterial::PushGraphicsData()
{
	CONSTANT_BUFFER(EConstantBufferType::Material)->PushGraphicsData(&Parameters, sizeof(Parameters));	// Upload CBV

	for (uint32 Index = 0; Index < Textures.size(); ++Index)	// Upload SRV
	{
		if (Textures[Index])
		{
			EShaderResourceViewRegisters Register = StaticCast< EShaderResourceViewRegisters >( StaticCast< int8 >( EShaderResourceViewRegisters::t0 ) + Index );
			GEngine->GetGraphicsResourceTables()->SetSRV(Textures[Index]->GetSRV(), Register);
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
			EShaderResourceViewRegisters Register = StaticCast<EShaderResourceViewRegisters>(StaticCast<uint8>(EShaderResourceViewRegisters::t0) + Index);
			GEngine->GetComputeResourceTables()->SetSRV(Textures[Index]->GetSRV(), Register);
		}
	}

	Shader->Update();
}

void FMaterial::Dispatch(uint32 X, uint32 Y, uint32 Z)
{
	PushComputeData();	// CBV + SRV + SetPipelineState

	GEngine->GetComputeResourceTables()->Commit();
	COMPUTE_COMMAND_LIST->Dispatch(X, Y, Z);
	GEngine->GetComputeCommandQueue()->Flush();
}
