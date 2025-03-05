#pragma once
// 클라이언트에서도 자주 사용되는 헤더를 모아놓는 용도
// include, lib, typedef 등

#define _HAS_STD_BYTE 0  // std::byte와 windows의 byte 정의가 겹치는 문제를 해결하기 위함

#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <filesystem>
using namespace std;
using FPaths = filesystem::path;

#include "d3dx12.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTexD.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using FVector2 = XMFLOAT2;
using FVector3 = XMFLOAT3;
using FVector4 = XMFLOAT4;
using FMatrix = XMMATRIX;

enum class EConstantBufferViewRegisters : uint8
{
	b0,
	b1,
	b2,
	b3,
	b4,

	END
};

enum class EShaderResourceViewRegisters : uint8
{
	// TableDescriptorHeap 구조 상 데이터들이 연속해서 등장해야 하기 때문에 겹치지 않는 번호로 세팅
	t0 = static_cast<uint32>(EConstantBufferViewRegisters::END),
	t1,
	t2,
	t3,
	t4,

	END
};

constexpr uint8 SWAP_CHAIN_BUFFER_COUNT = 2;
constexpr uint8 CBV_REGISTER_COUNT = static_cast<uint8>(EConstantBufferViewRegisters::END);
constexpr uint8 SRV_REGISTER_COUNT = static_cast<uint8>(EShaderResourceViewRegisters::END) - CBV_REGISTER_COUNT;
constexpr uint8 REGISTER_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT;

struct FWindowInfo
{
	HWND Window;		// 출력 윈도우
	int32 Width;		// 너비
	int32 Height;		// 높이
	bool bWindowed;		// 창모드 유무
};

struct FVertex
{
	FVector3 Position;	// 3차원 공간(x, y, z)
	FVector4 Color;		// RGBA
	FVector2 UV;		// UV 좌표계(== Texture 좌표. 3D 물체를 2D에 대응시키기 위해 사용)
};

struct FTransform
{
	FVector4 Offset;  // Shader에서 float4 offset 1개를 들고 있기 때문에 타입을 맞춰줌
};

#define DEVICE					GEngine->GetDevice()->GetD3DDevice()
#define COMMAND_LIST			GEngine->GetCommandQueue()->GetD3DCommandList()
#define ROOT_SIGNATURE			GEngine->GetRootSignature()->GetD3DRootSignature()
#define RESOURCE_COMMAND_LIST	GEngine->GetCommandQueue()->GetD3DResourceCommandList()

#define DELTA_TIME				GEngine->GetTimer()->GetDeltaTime()

extern unique_ptr<class Engine> GEngine;  // 전역에서 사용 가능한 Engine 클래스