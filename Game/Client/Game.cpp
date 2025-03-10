#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Shader.h"
#include "Texture.h"

shared_ptr<GameObject> Object = make_shared<GameObject>();

void Game::Initialize(const FWindowInfo& Info)
{
	GEngine->Initialize(Info);

	// 사각형은 삼각형 2개를 이용해 그리는 것이므로 정점 정보가 6개 있어야 함
	vector<FVertex> Vertices
	{
		{
			FVector3 ( -0.5f , 0.5f , 0.5f ),
			FVector4 ( 1.f , 0.f , 0.f , 1.f ),
			FVector2 ( 0.f , 0.f )
		},
		{
			FVector3 ( 0.5f , 0.5f , 0.5f ),
			FVector4 ( 0.f , 1.f , 0.f , 1.f ),
			FVector2 ( 1.f , 0.f )
		},
		{
			FVector3 ( 0.5f , -0.5f , 0.5f ),
			FVector4 ( 0.f , 0.f , 1.f , 1.f ),
			FVector2 ( 1.f , 1.f )
		},
		{
			FVector3 ( -0.5f , -0.5f , 0.5f ),
			FVector4 ( 0.f , 1.f , 0.f , 1.f ),
			FVector2 ( 0.f , 1.f )
		}
	};

	vector<uint32> Indices{0, 1, 2, 0, 2, 3};

	Object->Initialize ( );	// Add Transform Component

	// Test
	shared_ptr<FMeshRenderer> MeshRenderer = make_shared<FMeshRenderer>();
	{
		shared_ptr<FMesh> Mesh = make_shared<FMesh> ( );
		Mesh->Initialize ( Vertices , Indices );
		MeshRenderer->SetMesh ( Mesh );
	}
	{
		shared_ptr<FShader> Shader = make_shared<FShader> ( );
		shared_ptr<FTexture> Texture = make_shared<FTexture> ( );
		Shader->Initialize ( L"..\\Resources\\Shader\\Default.hlsli" );
		Texture->Initialize ( L"..\\Resources\\Texture\\F1.png" );

		shared_ptr<FMaterial> Material = make_shared<FMaterial> ( );
		Material->SetShader ( Shader );
		Material->SetMaterialParameters ( 0 , 0.3f );
		Material->SetMaterialParameters ( 1 , 0.4f );
		Material->SetMaterialParameters ( 2 , 0.3f );
		Material->SetTexture ( 0 , Texture );
		
		MeshRenderer->SetMaterial ( Material );
	}

	Object->AddComponent ( MeshRenderer );
	GEngine->GetCommandQueue()->WaitSync();
}

void Game::Update()
{

	// TEMP: 임시로 테스트를 위해 Begin, End 직접 호출
	GEngine->Update();

	GEngine->RenderBegin();	// Render는 Engine 내부에서 호출해줄 예정

	Object->Update( );	// TODO: Engine 단에서 수행

	GEngine->RenderEnd();
}
