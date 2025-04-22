#include "pch.h"
#include "Shader.h"

#include "Engine.h"

FShader::FShader()
	: Super(EObjectType::Shader)
	, Info()
	, GraphicsPipelineStateDesc()
	, ComputePipelineStateDesc()
{
}

FShader::~FShader() = default;

void FShader::CreateGraphicsShader(const wstring& Path, FShaderInfo InInfo, const string& VertexShaderInitter, const string& PixelShaderInitter, const string& GeometryShaderInitter)
{
	Info = InInfo;

	CreateVertexShader(Path, VertexShaderInitter, "vs_5_0");
	CreatePixelShader(Path, PixelShaderInitter, "ps_5_0");

	if(!GeometryShaderInitter.empty())
	{
		CreateGeometryShader(Path, GeometryShaderInitter, "gs_5_0");
	}
	
	D3D12_INPUT_ELEMENT_DESC Desc[]
	{
		// AlignedByteOffset: 0부터 시작하는 값의 오프셋 값. POSITION이 float3이라 COLOR가 12부터 시작, COLOR가 float4라 TEXCOORD가 12 + 16 = 28부터 시작
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

		// 행렬을 한 번에 넘기지 못해서 float 4개짜리를 4번씩 넘겨줌
		{"W", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"W", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"W", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"W", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"WV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"WV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"WV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"WV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 128, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 144, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 160, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{"WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 176, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
	};

	GraphicsPipelineStateDesc.InputLayout = { .pInputElementDescs = Desc, .NumElements = _countof(Desc) };
	GraphicsPipelineStateDesc.pRootSignature = GRAPHICS_ROOT_SIGNATURE.Get();

	GraphicsPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	GraphicsPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);	// 아래에서 Info.Blend 값에 따라 추가 설정
	GraphicsPipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// Default로 변경(depth = true, stencil = false)
	GraphicsPipelineStateDesc.SampleMask = UINT_MAX;
	GraphicsPipelineStateDesc.PrimitiveTopologyType = GetTopologyType(InInfo.Topology);
	GraphicsPipelineStateDesc.NumRenderTargets = 1;
	GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	GraphicsPipelineStateDesc.SampleDesc.Count = 1;
	GraphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;	// 변경되지 않음

	switch (InInfo.ShaderType)
	{
	case EShaderType::Deferred:
		GraphicsPipelineStateDesc.NumRenderTargets = NumRenderTargetGeometryBufferMember;
		GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// POSITION
		GraphicsPipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Normal
		GraphicsPipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;	// Color
		break;
	case EShaderType::Forward:
		GraphicsPipelineStateDesc.NumRenderTargets = 1;
		GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case EShaderType::Lighting:
		GraphicsPipelineStateDesc.NumRenderTargets = 2;
		GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		GraphicsPipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case EShaderType::Particle:
		GraphicsPipelineStateDesc.NumRenderTargets = 1;
		GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case EShaderType::Compute:
		GraphicsPipelineStateDesc.NumRenderTargets = 0;
		break;
	case EShaderType::Shadow:
		GraphicsPipelineStateDesc.NumRenderTargets = 1;
		GraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
		break;
	}

	switch (InInfo.RasterizeType)
	{
	case ERasterizeType::CullNone:
		GraphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GraphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case ERasterizeType::CullFront:
		GraphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GraphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case ERasterizeType::CullBack:
		GraphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GraphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case ERasterizeType::Wireframe:	// 외곽만 보임
		GraphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		GraphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	}

	switch (InInfo.DepthStencilType)
	{
	case EDepthStencilType::Less:
		GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
		GraphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case EDepthStencilType::LessEqual:
		GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
		GraphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case EDepthStencilType::Greater:
		GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
		GraphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		break;
	case EDepthStencilType::GreaterEqual:
		GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
		GraphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	case EDepthStencilType::NoDepth:
		GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = false;
		GraphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case EDepthStencilType::NoDepthNoWrite:
		GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = false;
		GraphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case EDepthStencilType::LessNoWrite:
		GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
		GraphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		GraphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	}

	/**
	 *	typedef struct D3D12_BLEND_DESC
	    {
			BOOL AlphaToCoverageEnable;
			BOOL IndependentBlendEnable;
			D3D12_RENDER_TARGET_BLEND_DESC RenderTarget[ 8 ];
	    } 	D3D12_BLEND_DESC;
	 *	한 번에 넘길 수 있는 렌더 타겟은 총 8개
	 *	IndependentBlendEnable == FALSE 이면 RenderTarget[0]만 사용하도록 제한
	 *	위에서 기본값으로 설정했기 때문에 False, 즉 0번만 꺼내와서 세팅
	 */
	D3D12_RENDER_TARGET_BLEND_DESC& RenderTarget = GraphicsPipelineStateDesc.BlendState.RenderTarget[0];

	// SrcBlend:	Pixel Shader
	// DestBlend:	Render Target
	switch (InInfo.BlendType)
	{
	case EBlendType::Default:
		RenderTarget.BlendEnable = false;
		RenderTarget.LogicOpEnable = false;
		RenderTarget.SrcBlend = D3D12_BLEND_ONE;	// (1, 1, 1, 1)
		RenderTarget.DestBlend = D3D12_BLEND_ZERO;	// (0, 0, 0, 0)
		break;
	case EBlendType::AlphaBlend:
		RenderTarget.BlendEnable = true;
		RenderTarget.LogicOpEnable = false;
		RenderTarget.SrcBlend = D3D12_BLEND_SRC_ALPHA;	// (A, A, A, A)
		RenderTarget.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	// (1-A, 1-A, 1-A, 1-A)
		break;
	case EBlendType::OneToOneBlend:
		RenderTarget.BlendEnable = true;
		RenderTarget.LogicOpEnable = false;
		RenderTarget.SrcBlend = D3D12_BLEND_ONE;
		RenderTarget.DestBlend = D3D12_BLEND_ONE;
		break;
	}

	assert(SUCCEEDED(DEVICE->CreateGraphicsPipelineState(&GraphicsPipelineStateDesc, IID_PPV_ARGS(&PipelineState))));
}

void FShader::CreateComputeShader(const wstring& Path, const string& Name, const string& Version)
{
	Info.ShaderType = EShaderType::Compute;

	Create(Path, Name, Version, ComputeShaderBlob, ComputePipelineStateDesc.CS);
	ComputePipelineStateDesc.pRootSignature = COMPUTE_ROOT_SIGNATURE.Get();

	assert(SUCCEEDED(DEVICE->CreateComputePipelineState(&ComputePipelineStateDesc, IID_PPV_ARGS(&PipelineState))));
}

void FShader::Update()
{
	if (GetShaderType() == EShaderType::Compute)
	{
		COMPUTE_COMMAND_LIST->SetPipelineState(PipelineState.Get());
	}
	else
	{
		// .fx 파일을 어떻게 읽고 사용할 지 정의한 PipelineState을 사용하겠다 선언
		GRAPHICS_COMMAND_LIST->IASetPrimitiveTopology(Info.Topology);
		GRAPHICS_COMMAND_LIST->SetPipelineState(PipelineState.Get());
	}
}

void FShader::Create(const wstring& Path, const string& Name, const string& Version, ComPtr<ID3DBlob>& Blob, D3D12_SHADER_BYTECODE& ShaderBytecode)
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
	Create(Path, Name, Version, VertexShaderBlob, GraphicsPipelineStateDesc.VS);
}

void FShader::CreatePixelShader(const wstring& Path, const string& Name, const string& Version)
{
	Create(Path, Name, Version, PixelShaderBlob, GraphicsPipelineStateDesc.PS);
}

void FShader::CreateGeometryShader(const wstring& Path, const string& Name, const string& Version)
{
	Create(Path, Name, Version, GeometryShaderBlob, GraphicsPipelineStateDesc.GS);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE FShader::GetTopologyType(D3D_PRIMITIVE_TOPOLOGY Topology)
{
	switch (Topology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	default:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
}