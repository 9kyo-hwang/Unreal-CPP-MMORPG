#include "pch.h"
#include "Resources.h"

#include "Engine.h"
#include "Shader.h"
#include "Texture.h"

void Resources::Initialize()
{
	CreateDefaultShader();
	CreateDefaultMaterial();
}

TSharedPtr<UMesh> Resources::LoadPoint()
{
	if (TSharedPtr<UMesh> Point = Get<UMesh>(L"Point"))
	{
		return Point;
	}

	vector<FVertex> Vertices{
		FVertex
		(
			FVector3(0, 0, 0),
			FVector2(0.5f, 0.5f),
			FVector3(0.f, 0.f, -1.f),
			FVector3(1.f, 0.f, 0.f)
		)
	};

	vector<uint32> Indices{0};

	TSharedPtr<UMesh> Point = MakeShared<UMesh>();
	Point->Initialize(Vertices, Indices);
	Add<UMesh>(L"Point", Point);

	return Point;
}

TSharedPtr<UMesh> Resources::LoadRectangle()
{
	if (TSharedPtr<UMesh> Rectangle = Get<UMesh>(L"Rectangle"))
	{
		return Rectangle;
	}

	float WidthHalf = 0.5f;
	float HeightHalf = 0.5f;

	// Cube에서 앞면만 그리면 Rectangle
	vector<FVertex> Vertices
	{
		// 앞면
		FVertex
		(
			FVector3(-WidthHalf, -HeightHalf, 0),
			FVector2(0.f, 1.f),
			FVector3(0.f, 0.f, -1.f),
			FVector3(1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(-WidthHalf, +HeightHalf, 0),
			FVector2(0.f, 0.f),
			FVector3(0.f, 0.f, -1.f),
			FVector3(1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, +HeightHalf, 0),
			FVector2(1.f, 0.f),
			FVector3(0.f, 0.f, -1.f),
			FVector3(1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, -HeightHalf, 0),
			FVector2(1.f, 1.f),
			FVector3(0.f, 0.f, -1.f),
			FVector3(1.f, 0.f, 0.f)
		),
	};

	vector<uint32> Indices{ 0, 1, 2, 0, 2, 3 };

	TSharedPtr<UMesh> Rectangle = MakeShared<UMesh>();
	Rectangle->Initialize(Vertices, Indices);
	Add<UMesh>(L"Rectangle", Rectangle);

	return Rectangle;
}

TSharedPtr<UMesh> Resources::LoadCube()
{
	if (TSharedPtr<UMesh> Cube = Get<UMesh>(L"Cube"))
	{
		return Cube;
	}

	// 크기가 (1, 1, 1)인 Cube를 만들기 위해 절반인 0.5 값을 이용
	float WidthHalf = 0.5f;
	float HeightHalf = 0.5f;
	float DepthHalf = 0.5f;

	vector<FVertex> Vertices
	{
		// 앞면
		FVertex
		(
			FVector3(-WidthHalf, -HeightHalf, -DepthHalf), 
			FVector2(0.f, 1.f), 
			FVector3(0.f, 0.f, -1.f), 
			FVector3(1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(-WidthHalf, +HeightHalf, -DepthHalf),
			FVector2(0.f, 0.f),
			FVector3(0.f, 0.f, -1.f),
			FVector3(1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, +HeightHalf, -DepthHalf),
			FVector2(1.f, 0.f),
			FVector3(0.f, 0.f, -1.f),
			FVector3(1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, -HeightHalf, -DepthHalf),
			FVector2(1.f, 1.f),
			FVector3(0.f, 0.f, -1.f),
			FVector3(1.f, 0.f, 0.f)
		),

		// 뒷면
		FVertex
		(
			FVector3(-WidthHalf, -HeightHalf, +DepthHalf),
			FVector2(1.f, 1.f),
			FVector3(0.f, 0.f, 1.f),
			FVector3(-1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, +HeightHalf, +DepthHalf),
			FVector2(0.f, 1.f),
			FVector3(0.f, 0.f, 1.f),
			FVector3(-1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, +HeightHalf, +DepthHalf),
			FVector2(0.f, 0.f),
			FVector3(0.f, 0.f, 1.f),
			FVector3(-1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(-WidthHalf, +HeightHalf, +DepthHalf),
			FVector2(1.f, 0.f),
			FVector3(0.f, 0.f, 1.f),
			FVector3(-1.f, 0.f, 0.f)
		),

		// 윗면
		FVertex
		(
			FVector3(-WidthHalf, +HeightHalf, -DepthHalf),
			FVector2(0.f, 1.f),
			FVector3(0.f, 1.f, 0.f),
			FVector3(1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, -HeightHalf, +DepthHalf),
			FVector2(0.f, 0.f),
			FVector3(0.f, 1.f, 0.f),
			FVector3(1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, +HeightHalf, +DepthHalf),
			FVector2(1.f, 0.f),
			FVector3(0.f, 1.f, 0.f),
			FVector3(1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, +HeightHalf, -DepthHalf),
			FVector2(1.f, 1.f),
			FVector3(0.f, 1.f, 0.f),
			FVector3(1.f, 0.f, 0.f)
		),

		// 아랫면
		FVertex
		(
			FVector3(-WidthHalf, -HeightHalf, -DepthHalf),
			FVector2(1.f, 1.f),
			FVector3(0.f, -1.f, 0.f),
			FVector3(-1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, -HeightHalf, -DepthHalf),
			FVector2(0.f, 1.f),
			FVector3(0.f, -1.f, 0.f),
			FVector3(-1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(+WidthHalf, -HeightHalf, +DepthHalf),
			FVector2(0.f, 0.f),
			FVector3(0.f, -1.f, 0.f),
			FVector3(-1.f, 0.f, 0.f)
		),
		FVertex
		(
			FVector3(-WidthHalf, -HeightHalf, +DepthHalf),
			FVector2(1.f, 0.f),
			FVector3(0.f, -1.f, 0.f),
			FVector3(-1.f, 0.f, 0.f)
		),

		// 왼쪽면
		FVertex
		(
		FVector3(-WidthHalf, -HeightHalf, +DepthHalf),
		FVector2(0.f, 1.f),
		FVector3(-1.f, 0.f, 0.f),
		FVector3(0.f, 0.f, -1.f)
		),
		FVertex
		(
		FVector3(-WidthHalf, +HeightHalf, +DepthHalf),
		FVector2(0.f, 0.f),
		FVector3(-1.f, 0.f, 0.f),
		FVector3(0.f, 0.f, -1.f)
		),
		FVertex
		(
		FVector3(-WidthHalf, +HeightHalf, -DepthHalf),
		FVector2(1.f, 0.f),
		FVector3(-1.f, 0.f, 0.f),
		FVector3(0.f, 0.f, -1.f)
		),
		FVertex
		(
		FVector3(-WidthHalf, -HeightHalf, -DepthHalf),
		FVector2(1.f, 1.f),
		FVector3(-1.f, 0.f, 0.f),
		FVector3(0.f, 0.f, -1.f)
		),

		// 오른쪽면
		FVertex
		(
		FVector3(+WidthHalf, -HeightHalf, -DepthHalf),
		FVector2(0.f, 1.f),
		FVector3(1.f, 0.f, 0.f),
		FVector3(0.f, 0.f, 1.f)
		),
		FVertex
		(
		FVector3(+WidthHalf, +HeightHalf, -DepthHalf),
		FVector2(0.f, 0.f),
		FVector3(1.f, 0.f, 0.f),
		FVector3(0.f, 0.f, 1.f)
		),
		FVertex
		(
		FVector3(+WidthHalf, +HeightHalf, +DepthHalf),
		FVector2(1.f, 0.f),
		FVector3(1.f, 0.f, 0.f),
		FVector3(0.f, 0.f, 1.f)
		),
		FVertex
		(
		FVector3(+WidthHalf, -HeightHalf, +DepthHalf),
		FVector2(1.f, 1.f),
		FVector3(1.f, 0.f, 0.f),
		FVector3(0.f, 0.f, 1.f)
		),
	};

	vector<uint32> Indices
	{
		0, 1, 2, 0, 2, 3,		// 앞면
		4, 5, 6, 4, 6, 7,		// 뒷면
		8, 9, 10, 8, 10, 11,	// 윗면
		12, 13, 14, 12, 14, 15,	// 아랫면
		16, 17, 18, 16, 18, 19,	// 왼쪽면
		20, 21, 22, 20, 22, 23,	// 오른쪽면
	};

	TSharedPtr<UMesh> Cube = MakeShared<UMesh>();
	Cube->Initialize(Vertices, Indices);
	Add<UMesh>(L"Cube", Cube);

	return Cube;
}

TSharedPtr<UMesh> Resources::LoadSphere()
{
	if (TSharedPtr<UMesh> Sphere = Get<UMesh>(L"Sphere"))
	{
		return Sphere;
	}

	float Radius = 0.5f;
	uint32 StackCount = 20;	// 가로 분할
	uint32 SliceCount = 20;	// 세로 분할

	vector<FVertex> Vertices;

	// 북극
	FVertex Vertex;
	Vertex.Position = FVector3(0.f, Radius, 0.f);
	Vertex.UV = FVector2(0.5f, 0.f);
	Vertex.Normal = Vertex.Position; Vertex.Normal.Normalize();
	Vertex.Tangent = FVector3(1.f, 0.f, 1.f);
	Vertices.push_back(Vertex);

	float StackAngle = XM_PI / StackCount;
	float SliceAngle = XM_2PI / SliceCount;

	// UV 값 세팅 시 사용될 증가량
	float DeltaU = 1.f / StaticCast<float>(SliceCount);
	float DeltaV = 1.f / StaticCast<float>(StackCount);

	// 각 고리를 순회하면서 정점 계산(북극/남극은 단일점이라 고리가 없음)
	for (uint32 Y = 1; Y <= StackCount - 1; ++Y)
	{
		float Phi = Y * StackAngle;

		// 고리에 위치한 정점
		for (uint32 X = 0; X <= SliceCount; ++X)
		{
			float Theta = X * SliceAngle;

			Vertex.Position.x = Radius * sinf(Phi) * cosf(Theta);
			Vertex.Position.y = Radius * cosf(Phi);
			Vertex.Position.z = Radius * sinf(Phi) * sinf(Theta);

			Vertex.UV = FVector2(DeltaU * X, DeltaV * Y);

			Vertex.Normal = Vertex.Position; Vertex.Normal.Normalize();

			Vertex.Tangent.x = -Radius * sinf(Phi) * sinf(Theta);
			Vertex.Tangent.y = 0.f;
			Vertex.Tangent.z = +Radius * sinf(Phi) * cosf(Theta);
			Vertex.Tangent.Normalize();

			Vertices.push_back(Vertex);
		}
	}

	// 남극
	Vertex.Position = FVector3(0.f, -Radius, 0.f);
	Vertex.UV = FVector2(0.5f, 1.f);
	Vertex.Normal = Vertex.Position; Vertex.Normal.Normalize();
	Vertex.Tangent = FVector3(1.f, 0.f, 0.f);
	Vertices.push_back(Vertex);

	vector<uint32> Indices;

	// 북극 인덱스
	for (uint32 Index = 0; Index <= SliceCount; ++Index)
	{
		/** 삼각형
		 *	[0]
		 *	 |	\
		 * [i+1]-[i+2]
		 */
		Indices.emplace_back(0);
		Indices.emplace_back(Index + 2);
		Indices.emplace_back(Index + 1);
	}

	// 몸통 인덱스
	uint32 RingVertexCount = SliceCount + 1;
	for (uint32 Y = 0; Y < StackCount - 2; ++Y)
	{
		for (uint32 X = 0; X < SliceCount; ++X)
		{
			/**
			 *	[y,   x] - [y, x+1]
			 *		|	  /
			 *	[y+1, x]
			 */

			Indices.emplace_back(1 + Y * RingVertexCount + X);
			Indices.emplace_back(1 + Y * RingVertexCount + ( X + 1 ));
			Indices.emplace_back(1 + ( Y + 1 ) * RingVertexCount + X);

			/**
			 *			    [y,   x+1]
			 *			  /		|
			 *	[y+1,  x] - [y+1, x+1]
			 */

			Indices.emplace_back(1 + ( Y + 1 ) * RingVertexCount + X);
			Indices.emplace_back(1 + Y * RingVertexCount + ( X + 1 ));
			Indices.emplace_back(1 + ( Y + 1 ) * RingVertexCount + ( X + 1 ));
		}
	}

	// 남극 인덱스
	uint32 BottomIndex = StaticCast<uint32>(Vertices.size() - 1);
	uint32 LastRingStartIndex = BottomIndex - RingVertexCount;
	for (uint32 Index = 0; Index < SliceCount; ++Index)
	{
		/**
		 *	[Last+i]-[Last+i+1]
		 *		|	/
		 *	[Bottom]
		 */

		Indices.emplace_back(BottomIndex);
		Indices.emplace_back(LastRingStartIndex + Index);
		Indices.emplace_back(LastRingStartIndex + Index + 1);
	}

	TSharedPtr<UMesh> Sphere = MakeShared<UMesh>();
	Sphere->Initialize(Vertices, Indices);
	Add<UMesh>(L"Sphere", Sphere);

	return Sphere;
}

TSharedPtr<FTexture> Resources::CreateTexture(const wstring& Name, DXGI_FORMAT Format, uint32 Width, uint32 Height,
	const D3D12_HEAP_PROPERTIES& HeapProperties, D3D12_HEAP_FLAGS HeapFlags, D3D12_RESOURCE_FLAGS ResourceFlags,
	FVector4 ClearColor)
{
	TSharedPtr<FTexture> Texture = MakeShared<FTexture>();
	Texture->Create(Format, Width, Height, HeapProperties, HeapFlags, ResourceFlags, ClearColor);
	Add<FTexture>(Name, Texture);

	return Texture;
}

TSharedPtr<FTexture> Resources::CreateTexture(const wstring& Name, ComPtr<ID3D12Resource> Texture2D)
{
	TSharedPtr<FTexture> Texture = MakeShared<FTexture>();
	Texture->Create(Texture2D);
	Add<FTexture>(Name, Texture);

	return Texture;
}

void Resources::CreateDefaultShader()
{
	// Skybox
	{
		TSharedPtr<FShader> Shader = MakeShared<FShader>();
		Shader->CreateGraphicsShader(
			L"..\\Resources\\Shader\\Skybox.fx",
			{ EShaderType::Forward, ERasterizeType::CullNone, EDepthStencilType::LessEqual }
		);

		Add<FShader>(L"Skybox", Shader);
	}

	// Deferred
	{
		TSharedPtr<FShader> Shader = MakeShared<FShader>();
		Shader->CreateGraphicsShader(
			L"..\\Resources\\Shader\\Deferred.fx",
			{ EShaderType::Deferred, }
		);
		Add<FShader>(L"Deferred", Shader);
	}

	// Forward(구 Default)
	{
		TSharedPtr<FShader> Shader = MakeShared<FShader>();
		Shader->CreateGraphicsShader(
			L"..\\Resources\\Shader\\Forward.fx", 
			{EShaderType::Forward, }
		);
		Add<FShader>(L"Forward", Shader);
	}

	// Texture
	{
		TSharedPtr<FShader> Shader = MakeShared<FShader>();
		Shader->CreateGraphicsShader(
			L"..\\Resources\\Shader\\Forward.fx",
			{EShaderType::Forward, ERasterizeType::CullNone, EDepthStencilType::NoDepthNoWrite},
			"VSTex", "PSTex"
		);
		Add<FShader>(L"Texture", Shader);
	}

	// Directional Light
	{
		TSharedPtr<FShader> Shader = MakeShared<FShader>();
		Shader->CreateGraphicsShader(
			L"..\\Resources\\Shader\\Lighting.fx",
			{ EShaderType::Lighting, ERasterizeType::CullNone, EDepthStencilType::NoDepthNoWrite, EBlendType::OneToOneBlend },
			"VSDirectionalLight", "PSDirectionalLight"
		);
		Add<FShader>(L"DirectionalLight", Shader);
	}

	// Point Light
	{
		// CullNone으로 세팅해줘야 빛의 범위가 매우 커지더라도 정상적으로 표시됨
		TSharedPtr<FShader> Shader = MakeShared<FShader>();
		Shader->CreateGraphicsShader(
			L"..\\Resources\\Shader\\Lighting.fx",
			{ EShaderType::Lighting, ERasterizeType::CullNone, EDepthStencilType::NoDepthNoWrite, EBlendType::OneToOneBlend },
			"VSPointLight", "PSPointLight"
		);
		Add<FShader>(L"PointLight", Shader);
	}

	// Final
	{
		TSharedPtr<FShader> Shader = MakeShared<FShader>();
		Shader->CreateGraphicsShader(
			L"..\\Resources\\Shader\\Lighting.fx",
			{ EShaderType::Lighting, ERasterizeType::CullBack, EDepthStencilType::NoDepthNoWrite },
			"VSFinal", "PSFinal"
		);
		Add<FShader>(L"Final", Shader);
	}

	// Compute Shader
	{
		TSharedPtr<FShader> Shader = MakeShared<FShader>();
		Shader->CreateComputeShader(L"..\\Resources\\Shader\\Compute.fx", "CSMain", "cs_5_0");
		Add<FShader>(L"ComputeShader", Shader);
	}

	//// Particle
	//{
	//	shared_ptr<FShader> Shader = make_shared<FShader>();
	//	Shader->CreateGraphicsShader(
	//		L"..\\Resources\\Shader\\Particle.fx",
	//		{EShaderType::Particle, ERasterizeType::CullBack, EDepthStencilType::LessNoWrite, EBlendType::AlphaBlend, D3D_PRIMITIVE_TOPOLOGY_POINTLIST},
	//		"VSMain", "PSMain", "GSMain"
	//		);
	//	Add<FShader>(L"Particle", Shader);
	//}

	//// Compute Particle
	//{
	//	shared_ptr<FShader> Shader = make_shared<FShader>();
	//	Shader->CreateComputeShader(
	//		L"..\\Resources\\Shader\\Particle.fx",
	//		"CSMain", "cs_5_0"
	//		);
	//	Add<FShader>(L"ComputeParticle", Shader);
	//}
}

void Resources::CreateDefaultMaterial()
{
	// Skybox
	{
		TSharedPtr<FShader> Shader = Get()->Get<FShader>(L"Skybox");
		TSharedPtr<FMaterial> Material = MakeShared<FMaterial>();
		Material->SetShader(Shader);

		Add<FMaterial>(L"Skybox", Material);
	}

	// Directional Light
	{
		TSharedPtr<FShader> Shader = Get()->Get<FShader>(L"DirectionalLight");
		TSharedPtr<FMaterial> Material = MakeShared<FMaterial>();
		Material->SetShader(Shader);
		Material->SetTexture(0, Get()->Get<FTexture>(L"PositionTarget"));	// MRT 내부에서 Create된 상태
		Material->SetTexture(1, Get()->Get<FTexture>(L"NormalTarget"));

		Add<FMaterial>(L"DirectionalLight", Material);
	}

	// Point Light
	{
		const FWindowInfo& Info = GEngine->GetWindow();
		FVector2 Resolution = { StaticCast<float>(Info.Width), StaticCast<float>(Info.Height) };

		TSharedPtr<FShader> Shader = Get()->Get<FShader>(L"PointLight");

		TSharedPtr<FMaterial> Material = MakeShared<FMaterial>();
		Material->SetShader(Shader);
		Material->SetTexture(0, Get()->Get<FTexture>(L"PositionTarget"));
		Material->SetTexture(1, Get()->Get<FTexture>(L"NormalTarget"));
		Material->SetParameter(0, Resolution);

		Add<FMaterial>(L"PointLight", Material);
	}

	// Final
	{
		TSharedPtr<FShader> Shader = Get()->Get<FShader>(L"Final");

		TSharedPtr<FMaterial> Material = MakeShared<FMaterial>();
		Material->SetShader(Shader);
		Material->SetTexture(0, Get()->Get<FTexture>(L"DiffuseTarget"));
		Material->SetTexture(1, Get()->Get<FTexture>(L"DiffuseLightTarget"));
		Material->SetTexture(2, Get()->Get<FTexture>(L"SpecularLightTarget"));

		Add<FMaterial>(L"Final", Material);
	}

	// Compute Shader
	{
		TSharedPtr<FShader> Shader = Get()->Get<FShader>(L"ComputeShader");
		TSharedPtr<FMaterial> Material = MakeShared<FMaterial>();	// 셰이더에게 인자를 넘길 때 활용
		Material->SetShader(Shader);
		Add<FMaterial>(L"ComputeShader", Material);
	}

	//// Particle
	//{
	//	shared_ptr<FShader> Shader = Get()->Get<FShader>(L"Particle");
	//	shared_ptr<FMaterial> Material = make_shared<FMaterial>();
	//	Material->SetShader(Shader);
	//	Add<FMaterial>(L"Particle", Material);
	//}

	//// ComputeParticle
	//{
	//	shared_ptr<FShader> Shader = Get()->Get<FShader>(L"ComputeParticle");
	//	shared_ptr<FMaterial> Material = make_shared<FMaterial>();
	//	Material->SetShader(Shader);

	//	Add<FMaterial>(L"ComputeParticle", Material);
	//}

	// Actor
	{
		TSharedPtr<FShader> Shader = Get()->Get<FShader>(L"Deferred");	// Deferred로 변경
		TSharedPtr<FTexture> Texture = Get()->Load<FTexture>(L"Wood", L"..\\Resources\\Texture\\Wood.jpg");
		TSharedPtr<FTexture> NormalTexture = Get()->Load<FTexture>(L"Wood_Normal", L"..\\Resources\\Texture\\Wood_Normal.jpg");

		TSharedPtr<FMaterial> Material = MakeShared<FMaterial>();
		Material->SetShader(Shader);
		Material->SetTexture(0, Texture);
		Material->SetTexture(1, NormalTexture);

		Add<FMaterial>(L"GameObject", Material);
	}
}
