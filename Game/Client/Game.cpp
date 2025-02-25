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

	vector<FVertex> Vertices
	{
		FVertex(FVector3(0.f, 0.5f, 0.f), FVector4(1.f, 0.f, 0.f, 1.f)),
		FVertex(FVector3(0.5f, -0.5f, 0.5f), FVector4(0.f, 1.f, 0.f, 1.f)),
		FVertex(FVector3(-0.5f, -0.5f, 0.5f), FVector4(0.f, 0.f, 1.f, 1.f)),
	};

	Mesh->Initialize(Vertices);
	Shader->Initialize(L"..\\Resources\\Shader\\Default.hlsli");

	GEngine->GetCommandQueue()->WaitSync();
}

void Game::Update()
{
	// TEMP: 임시로 테스트를 위해 Begin, End 직접 호출
	GEngine->RenderBegin();

	Shader->Update();
	{
		// Shader의 cbuffer TEST_B0에 들어감
		FTransform Transform{ FVector4 ( 0.75f, 0.f, 0.f, 0.f ) };
		Mesh->SetTransform(Transform);
		Mesh->Render();

		// x좌표 + 0.75, R 컬러 + 0.75(Shader Main에서 pos, color += offset을 수행하므로)
	}
	{
		// Shader의 cbuffer TEST_B1에 들어감
		FTransform Transform{ FVector4 ( 0.f, 0.75f, 0.f, 0.f ) };
		Mesh->SetTransform(Transform);
		Mesh->Render();

		// y좌표 + 0.75, G 컬러 + 0.75
	}
	Mesh->Render();

	GEngine->RenderEnd();
}
