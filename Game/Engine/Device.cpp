#include "pch.h"
#include "Device.h"

void FDevice::Initialize()
{
	/*
	 *	D3D12 ����� ���̾� Ȱ��ȭ
	 *	VC++ ���â�� ���� ����� �޽��� ���
	 *	riid: ����̽��� COM ID
	 *	ppDevice: ������ ��ġ�� �Ű������� ����
	 */
#ifdef _DEBUG
	::D3D12GetDebugInterface(IID_PPV_ARGS(&DebugContainer));
	DebugContainer->EnableDebugLayer();
#endif

	/*
	 *	riid: ����̽��� COM ID
	 *	ppDevice: ������ ��ġ�� �Ű������� ����
	 */
	::CreateDXGIFactory(IID_PPV_ARGS(&DXGI));

	/*
	 *	���÷��� �����(GPU)�� ��Ÿ���� ��ü
	 *	pAdapter: nullptr�� �����ϸ� �ý��� �⺻ GPU ���
	 *	MinimumFeatureLevel: Application�� �䱸�ϴ� �ּ� ����
	 *	riid: ����̽� COM ID
	 *	ppDevice: ������ ��ġ�� �Ű������� ����
	 */
	::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device));
}
