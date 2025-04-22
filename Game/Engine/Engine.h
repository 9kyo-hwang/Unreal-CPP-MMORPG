#pragma once

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "ConstantBuffer.h"
#include "MultipleRenderTarget.h"
#include "TableDescriptorHeap.h"

class Engine
{
public:
	Engine();
	~Engine();

	// Client의 Initialize를 호출할 때 함께 호출됨
	void Initialize(const FWindowInfo& InInfo);
	void Update();
	void Render();

public:
	void PreRender();
	void PostRender();
	void ResizeWindow(int32 Width, int32 Height);

public:
	FWindowInfo GetWindow() const { return Info; }

	TSharedPtr<FDevice> GetDevice() { return Device; }
	TSharedPtr<FGraphicsCommandQueue> GetGraphicsCommandQueue() { return GraphicsCommandQueue; }
	TSharedPtr<FComputeCommandQueue> GetComputeCommandQueue() { return ComputeCommandQueue; }
	TSharedPtr<FSwapChain> GetSwapChain() { return SwapChain; }
	TSharedPtr<FGraphicsRootSignature> GetGraphicsRootSignature() { return GraphicsRootSignature; }
	TSharedPtr<FComputeRootSignature> GetComputeRootSignature() { return ComputeRootSignature; }
	TSharedPtr<FGraphicsResourceTables> GetGraphicsResourceTables() { return GraphicsResourceTables; }
	TSharedPtr<FComputeResourceTables> GetComputeResourceTables() { return ComputeResourceTables; }
	TSharedPtr<FConstantBuffer> GetConstantBuffer(EConstantBufferType Type) { return ConstantBufferList[StaticCast<uint8>(Type)]; }
	TSharedPtr<FMultipleRenderTarget> GetMultipleRenderTarget(ERenderTargetType Type) { return MultipleRenderTargetArray[StaticCast<uint8>(Type)]; }

private:
	void ShowFPS();
	void CreateConstantBuffer(EConstantBufferViewRegisters Register, uint32 BufferSize, uint32 Count);
	void CreateMultipleRenderTargets();

private:
	FWindowInfo Info;
	D3D12_VIEWPORT Viewport;
	D3D12_RECT ScissorRect;

	TSharedPtr<FDevice> Device;
	TSharedPtr<FGraphicsCommandQueue> GraphicsCommandQueue;
	TSharedPtr<FComputeCommandQueue> ComputeCommandQueue;
	TSharedPtr<FSwapChain> SwapChain;
	TSharedPtr<FGraphicsRootSignature> GraphicsRootSignature;
	TSharedPtr<FComputeRootSignature> ComputeRootSignature;
	TSharedPtr<FGraphicsResourceTables> GraphicsResourceTables;
	TSharedPtr<FComputeResourceTables> ComputeResourceTables;

	vector<TSharedPtr<FConstantBuffer>> ConstantBufferList;
	array<TSharedPtr<FMultipleRenderTarget>, NumRenderTargets> MultipleRenderTargetArray;	// SwapChain, GBuffer
};

