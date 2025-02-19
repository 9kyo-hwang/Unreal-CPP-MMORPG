#pragma once

class Engine
{
public:
	// Client�� Initialize�� ȣ���� �� �Բ� ȣ���
	void Initialize(const FWindowInfo& InInfo);
	void Render();

public:
	void RenderBegin();
	void RenderEnd();

	void ResizeWindow(int32 Width, int32 Height);

private:
	FWindowInfo Info{};
	D3D12_VIEWPORT Viewport{};
	D3D12_RECT ScissorRect{};

	shared_ptr<class FDevice> Device;
	shared_ptr<class FCommandQueue> CommandQueue;
	shared_ptr<class FSwapChain> SwapChain;
	shared_ptr<class FDescriptorHeap> DescriptorHeap;
};

