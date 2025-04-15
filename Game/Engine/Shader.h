#pragma once

#include "Object.h"

enum class EShaderType : uint8 // 해당 셰이더를 어떤 방식으로 그려야할 지 구분해줘야 함
{
	Deferred,
	Forward,
	Lighting,
	Compute,
};

enum class ERasterizeType : uint8
{
	CullNone,
	CullFront,
	CullBack,	// Default
	Wireframe,
};

enum class EDepthStencilType : uint8
{
	Less,	// Default
	LessEqual,
	Greater,
	GreaterEqual,

	NoDepthWrite,	// 깊이 테스트 X & 기록 O
	NoDepthNoWrite,	// 깊이 테스트 & 기록 X
	LessNoWrite,	// 깊이 테스트 O & 기록 X
};

enum class EBlendType
{
	Default,
	AlphaBlend,
	OneToOneBlend,
	END
};

struct FShaderInfo
{
	EShaderType ShaderType = EShaderType::Forward;
	ERasterizeType RasterizeType = ERasterizeType::CullBack;
	EDepthStencilType DepthStencilType = EDepthStencilType::Less;  // Skybox는 1에 위치해도 그려야하므로
	EBlendType BlendType = EBlendType::Default;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
};

class FShader : public Object
{
	using Super = Object;

public:
	FShader();
	~FShader() override;

	// 외부의 파일로 관리하게 될 예정, 그것들을 로드
	void CreateGraphicsShader(const wstring& Path, FShaderInfo InInfo = {}, const string& VertexShaderInitter = "VSMain", const string& PixelShaderInitter = "PSMain");
	void CreateComputeShader(const wstring& Path, const string& Name, const string& Version);
	void Update();

	EShaderType GetShaderType() const { return Info.ShaderType; }

private:
	void Create(const wstring& Path, const string& Name, const string& Version, ComPtr<ID3DBlob>& Blob, D3D12_SHADER_BYTECODE& ShaderBytecode);
	void CreateVertexShader(const wstring& Path, const string& Name, const string& Version);
	void CreatePixelShader(const wstring& Path, const string& Name, const string& Version);

private:
	FShaderInfo Info;
	ComPtr<ID3D12PipelineState> PipelineState;

private:
	ComPtr<ID3DBlob> VertexShaderBlob;
	ComPtr<ID3DBlob> PixelShaderBlob;
	ComPtr<ID3DBlob> ErrorBlob;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineStateDesc;

private:
	ComPtr<ID3DBlob> ComputeShaderBlob;
	D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePipelineStateDesc;
};