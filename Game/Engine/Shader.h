#pragma once

class FShader
{
public:
	// �ܺ��� ���Ϸ� �����ϰ� �� ����, �װ͵��� �ε�
	void Initialize(const wstring& Path);
	void Update();

private:
	void CreateShader(const wstring& Path, const string& Name, const string& Version, ComPtr<ID3DBlob>& Blob, D3D12_SHADER_BYTECODE& ShaderBytecode);
	void CreateVertexShader(const wstring& Path, const string& Name, const string& Version);
	void CreatePixelShader(const wstring& Path, const string& Name, const string& Version);

private:
	ComPtr<ID3DBlob> VertexShaderBlob;
	ComPtr<ID3DBlob> PixelShaderBlob;
	ComPtr<ID3DBlob> ErrorBlob;

	ComPtr<ID3D12PipelineState> PipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc{};
};

