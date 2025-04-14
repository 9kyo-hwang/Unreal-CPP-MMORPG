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
	void RenderBegin();
	void RenderEnd();
	void ResizeWindow(int32 Width, int32 Height);

public:
	FWindowInfo GetWindow() const { return Info; }

	shared_ptr<FDevice> GetDevice() { return Device; }
	shared_ptr<FCommandQueue> GetCommandQueue() { return CommandQueue; }
	shared_ptr<FSwapChain> GetSwapChain() { return SwapChain; }
	shared_ptr<FRootSignature> GetRootSignature() { return RootSignature; }
	shared_ptr<FTableDescriptorHeap> GetTableDescriptorHeap() { return TableDescriptorHeap; }
	shared_ptr<FConstantBuffer> GetConstantBuffer(EConstantBufferType Type) { return ConstantBufferList[static_cast<uint8>(Type)]; }
	shared_ptr<MultipleRenderTarget> GetMultipleRenderTarget(EMultipleRenderTargetType Type) { return MultipleRenderTargetArray[static_cast<uint8>(Type)]; }

private:
	void ShowFPS();
	void CreateConstantBuffer(EConstantBufferViewRegisters Register, uint32 BufferSize, uint32 Count);
	void CreateMultipleRenderTargets();

private:
	FWindowInfo Info;
	D3D12_VIEWPORT Viewport;
	D3D12_RECT ScissorRect;

	shared_ptr<FDevice> Device;
	shared_ptr<FCommandQueue> CommandQueue;
	shared_ptr<FSwapChain> SwapChain;
	shared_ptr<FRootSignature> RootSignature;
	shared_ptr<FTableDescriptorHeap> TableDescriptorHeap;

	vector<shared_ptr<FConstantBuffer>> ConstantBufferList;
	array<shared_ptr<MultipleRenderTarget>, NumMultipleRenderTarget> MultipleRenderTargetArray;	// SwapChain, GBuffer
};

