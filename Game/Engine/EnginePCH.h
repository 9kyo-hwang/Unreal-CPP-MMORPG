#pragma once
// 클라이언트에서도 자주 사용되는 헤더를 모아놓는 용도
// include, lib, typedef 등

#define _HAS_STD_BYTE 0  // std::byte와 windows의 byte 정의가 겹치는 문제를 해결하기 위함

#include "Aliases.h"
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
#include "SimpleMath.h"
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
using FVector2 = DirectX::SimpleMath::Vector2;
using FVector3 = DirectX::SimpleMath::Vector3;
using FVector4 = DirectX::SimpleMath::Vector4;
using FMatrix = DirectX::SimpleMath::Matrix;

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
	t0 = ConstexprCast<uint32>(EConstantBufferViewRegisters::END),
	t1,
	t2,
	t3,
	t4,	// 확장
	t5,	
	t6,
	t7,
	t8,
	t9,

	END
};

enum class EUnorderedAccessViewRegisters : uint8
{
	u0 = ConstexprCast<uint8>(EShaderResourceViewRegisters::END),
	u1,
	u2,
	u3,
	u4,

	END
};

constexpr uint8 NumSwapChainBuffer = 2;
constexpr uint8 NumCBVRegister = ConstexprCast<uint8>(EConstantBufferViewRegisters::END);
constexpr uint8 NumSRVRegister = ConstexprCast<uint8>(EShaderResourceViewRegisters::END) - NumCBVRegister;
constexpr uint8 NumUAVRegister = ConstexprCast<uint8>(EUnorderedAccessViewRegisters::END) - NumSRVRegister - NumCBVRegister;
constexpr uint8 NumCBVSRVRegister = NumCBVRegister + NumSRVRegister;
constexpr uint8 NumTotalRegister = NumCBVRegister + NumSRVRegister + NumUAVRegister;

struct FWindowInfo
{
	HWND Window;		// 출력 윈도우
	int32 Width;		// 너비
	int32 Height;		// 높이
	bool bWindowed;		// 창모드 유무
};

struct FVertex
{
	FVertex() = default;

	FVertex(FVector3 InPosition, FVector2 InUV, FVector3 InNormal, FVector3 InTangent)
		: Position(InPosition)
		, UV(InUV)
		, Normal(InNormal)
		, Tangent(InTangent)
	{}

	FVector3 Position;	// 3차원 공간(x, y, z)
	FVector2 UV;		// UV 좌표계(== Texture 좌표. 3D 물체를 2D에 대응시키기 위해 사용)

	// Lighting을 위해 필요한 정보
	FVector3 Normal;
	FVector3 Tangent;
};

struct FTransformParameters
{
	// WVP Matrix 외에도 중간 단계 행렬이 필요할 경우를 대비해 같이 정보를 담도록 수정
	FMatrix WorldMatrix;
	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix WorldViewMatrix;
	FMatrix WorldViewProjectionMatrix;
	FMatrix InverseViewMatrix;
};

#define GENERATED_SINGLETON(type)	\
public:								\
	static type* Get()				\
	{								\
		static type Instance;		\
		return &Instance;			\
	}								\
private:							\
	type() {}						\
	~type() {}						\

#define DEVICE					GEngine->GetDevice()->GetDevice()
#define GRAPHICS_COMMAND_LIST	GEngine->GetGraphicsCommandQueue()->GetCommandList()
#define RESOURCE_COMMAND_LIST	GEngine->GetGraphicsCommandQueue()->GetResourceCommandList()
#define COMPUTE_COMMAND_LIST	GEngine->GetComputeCommandQueue()->GetCommandList()
#define GRAPHICS_ROOT_SIGNATURE	GEngine->GetGraphicsRootSignature()->GetRootSignature()
#define COMPUTE_ROOT_SIGNATURE	GEngine->GetComputeRootSignature()->GetRootSignature()
#define CONSTANT_BUFFER(Type)	GEngine->GetConstantBuffer(Type)

extern TUniquePtr<class Engine> GEngine;  // 전역에서 사용 가능한 Engine 클래스