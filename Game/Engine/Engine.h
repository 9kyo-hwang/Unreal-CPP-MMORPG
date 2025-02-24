#pragma once

class FDevice;
class FCommandQueue;
class FSwapChain;

class Engine
{
public:
	// Client의 Initialize를 호출할 때 함께 호출됨
	void Initialize(const FWindowInfo& InInfo);
	void Render();

public:
	void RenderBegin();
	void RenderEnd();
	void ResizeWindow(int32 Width, int32 Height);

public:
	shared_ptr<FDevice> GetDevice() { return Device; }
	shared_ptr<FCommandQueue> GetCommandQueue() { return CommandQueue; }
	shared_ptr<FSwapChain> GetSwapChain() { return SwapChain; }

private:
	FWindowInfo Info{};
	D3D12_VIEWPORT Viewport{};
	D3D12_RECT ScissorRect{};

	shared_ptr<FDevice> Device;
	shared_ptr<FCommandQueue> CommandQueue;
	shared_ptr<FSwapChain> SwapChain;
};

