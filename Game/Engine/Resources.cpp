#include "pch.h"
#include "Resources.h"

#include "Shader.h"

void Resources::Initialize()
{
	CreateDefaultShader();
}

shared_ptr<FMesh> Resources::LoadRectangle()
{
	if (shared_ptr<FMesh> Rectangle = Get<FMesh>(L"Rectangle"))
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

	shared_ptr<FMesh> Rectangle = make_shared<FMesh>();
	Rectangle->Initialize(Vertices, Indices);
	Add<FMesh>(L"Rectangle", Rectangle);

	return Rectangle;
}

shared_ptr<FMesh> Resources::LoadCube()
{
	if (shared_ptr<FMesh> Cube = Get<FMesh>(L"Cube"))
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

	shared_ptr<FMesh> Cube = make_shared<FMesh>();
	Cube->Initialize(Vertices, Indices);
	Add<FMesh>(L"Cube", Cube);

	return Cube;
}

shared_ptr<FMesh> Resources::LoadSphere()
{
	if (shared_ptr<FMesh> Sphere = Get<FMesh>(L"Sphere"))
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
	float DeltaU = 1.f / static_cast<float>(SliceCount);
	float DeltaV = 1.f / static_cast<float>(StackCount);

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
	uint32 BottomIndex = static_cast<uint32>(Vertices.size() - 1);
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

	shared_ptr<FMesh> Sphere = make_shared<FMesh>();
	Sphere->Initialize(Vertices, Indices);
	Add<FMesh>(L"Sphere", Sphere);

	return Sphere;
}

void Resources::CreateDefaultShader()
{
	// Skybox
	{
		shared_ptr<FShader> Shader = make_shared<FShader>();
		Shader->Initialize(
			L"..\\Resources\\Shader\\Skybox.fx",
			{ ERasterizeType::CullNone, EDepthStencilType::LessEqual }
		);

		Add<FShader>(L"Skybox", Shader);
	}

	// Forward(구 Default)
	{
		shared_ptr<FShader> Shader = make_shared<FShader>();
		Shader->Initialize(L"..\\Resources\\Shader\\Forward.fx", {});
		Add<FShader>(L"Forward", Shader);
	}
}
