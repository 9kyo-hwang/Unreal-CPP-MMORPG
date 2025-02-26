#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "Mesh.h"
#include "Shader.h"

shared_ptr<FMesh> Mesh = make_shared<FMesh>();
shared_ptr<FShader> Shader = make_shared<FShader>();

void Game::Initialize(const FWindowInfo& Info)
{
	GEngine->Initialize(Info);

	// 사각형은 삼각형 2개를 이용해 그리는 것이므로 정점 정보가 6개 있어야 함
	vector<FVertex> Vertices
	{
		FVertex(FVector3(-0.5f, 0.5f, 0.5f), FVector4(1.f, 0.f, 0.f, 1.f)),
		FVertex(FVector3(0.5f, 0.5f, 0.5f), FVector4(0.f, 1.f, 0.f, 1.f)),
		FVertex(FVector3(0.5f, -0.5f, 0.5f), FVector4(0.f, 0.f, 1.f, 1.f)),
		FVertex ( FVector3 ( -0.5f, -0.5f, 0.5f ), FVector4 ( 0.f, 1.f, 0.f, 1.f ) ),
	};

	vector<uint32> Indices{0, 1, 2, 0, 2, 3};

	Mesh->Initialize(Vertices, Indices);
	Shader->Initialize(L"..\\Resources\\Shader\\Default.hlsli");

	GEngine->GetCommandQueue()->WaitSync();
}

void Game::Update()
{
	// TEMP: 임시로 테스트를 위해 Begin, End 직접 호출
	GEngine->RenderBegin();

	Shader->Update();
	{
		FTransform Transform{ FVector4 ( 0.f, 0.f, 0.f, 0.f ) };
		Mesh->SetTransform(Transform);
		Mesh->Render();
	}

	GEngine->RenderEnd();
}
