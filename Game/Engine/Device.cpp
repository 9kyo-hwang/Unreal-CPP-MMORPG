#include "pch.h"
#include "Device.h"

void FDevice::Initialize()
{
	/*
	 *	D3D12 디버그 레이어 활성화
	 *	VC++ 출력창에 상세한 디버깅 메시지 출력
	 *	riid: 디바이스의 COM ID
	 *	ppDevice: 생성된 장치가 매개변수에 설정
	 */
#ifdef _DEBUG
	::D3D12GetDebugInterface(IID_PPV_ARGS(&DebugContainer));
	DebugContainer->EnableDebugLayer();
#endif

	/*
	 *	riid: 디바이스의 COM ID
	 *	ppDevice: 생성된 장치가 매개변수에 설정
	 */
	::CreateDXGIFactory(IID_PPV_ARGS(&DXGI));

	/*
	 *	디스플레이 어댑터(GPU)를 나타내는 객체
	 *	pAdapter: nullptr을 지정하면 시스템 기본 GPU 사용
	 *	MinimumFeatureLevel: Application이 요구하는 최소 버전
	 *	riid: 디바이스 COM ID
	 *	ppDevice: 생성된 장치가 매개변수에 설정
	 */
	::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device));
}
