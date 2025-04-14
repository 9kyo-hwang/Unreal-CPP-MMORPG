#include "pch.h"
#include "Shader.h"

#include "Engine.h"

FShader::FShader()
	: Super(EObjectType::Shader)
{
}

FShader::~FShader()
{
}

void FShader::Initialize(const wstring& Path, FShaderInfo InInfo)
{
	Info = InInfo;

	CreateVertexShader(Path, "VS_Main", "vs_5_0");
	CreatePixelShader(Path, "PS_Main", "ps_5_0");

	D3D12_INPUT_ELEMENT_DESC Desc[]
	{
		// AlignedByteOffset: 0부터 시작하는 값의 오프셋 값. POSITION이 float3이라 COLOR가 12부터 시작, COLOR가 float4라 TEXCOORD가 12 + 16 = 28부터 시작
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	PipelineStateDesc.InputLayout = { .pInputElementDescs = Desc, .NumElements = _countof(Desc) };
	PipelineStateDesc.pRootSignature = ROOT_SIGNATURE.Get();

	PipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	PipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// Default로 변경(depth = true, stencil = false)
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = InInfo.Topology;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.SampleDesc.Count = 1;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;	// 변경되지 않음

	switch (InInfo.Shader)
	{
	case EShaderType::Deferred:
		PipelineStateDesc.NumRenderTargets = NumRenderTargetGeometryBufferMember;
		PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// POSITION
		PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Normal
		PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;	// Color
		break;
	case EShaderType::Forward:
		PipelineStateDesc.NumRenderTargets = 1;
		PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}

	switch (InInfo.Rasterize)
	{
	case ERasterizeType::CullNone:
		PipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		PipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case ERasterizeType::CullFront:
		PipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		PipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case ERasterizeType::CullBack:
		PipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		PipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case ERasterizeType::Wireframe:	// 외곽만 보임
		PipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		PipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	}

	switch (InInfo.DepthStencil)
	{
	case EDepthStencilType::Less:
		PipelineStateDesc.DepthStencilState.DepthEnable = true;
		PipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case EDepthStencilType::LessEqual:
		PipelineStateDesc.DepthStencilState.DepthEnable = true;
		PipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case EDepthStencilType::Greater:
		PipelineStateDesc.DepthStencilState.DepthEnable = true;
		PipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		break;
	case EDepthStencilType::GreaterEqual:
		PipelineStateDesc.DepthStencilState.DepthEnable = true;
		PipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	}

	DEVICE->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&PipelineState));
}

void FShader::Update()
{
	// .hlsli 파일을 어떻게 읽고 사용할 지 정의한 PipelineState을 사용하겠다 선언
	COMMAND_LIST->SetPipelineState(PipelineState.Get());
}

void FShader::CreateShader(const wstring& Path, const string& Name, const string& Version, ComPtr<ID3DBlob>& Blob, D3D12_SHADER_BYTECODE& ShaderBytecode)
{
	uint32 CompileFlag = 0;
#ifdef _DEBUG
	CompileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		Name.c_str(), Version.c_str(), CompileFlag, 0, &Blob, &ErrorBlob)))
	{
		::MessageBoxA(nullptr, "Shader Create Failed!", nullptr, MB_OK);
	}

	ShaderBytecode = { Blob->GetBufferPointer(), Blob->GetBufferSize() };
}

void FShader::CreateVertexShader(const wstring& Path, const string& Name, const string& Version)
{
	CreateShader(Path, Name, Version, VertexShaderBlob, PipelineStateDesc.VS);
}

void FShader::CreatePixelShader(const wstring& Path, const string& Name, const string& Version)
{
	CreateShader(Path, Name, Version, PixelShaderBlob, PipelineStateDesc.PS);
}
