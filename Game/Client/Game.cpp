#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

shared_ptr<FMesh> Mesh = make_shared<FMesh>();

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

	Mesh->Initialize(Vertices, Indices);

	shared_ptr<FShader> Shader = make_shared<FShader> ( );
	shared_ptr<FTexture> Texture = make_shared<FTexture> ( );
	Shader->Initialize(L"..\\Resources\\Shader\\Default.hlsli");
	Texture->Initialize(L"..\\Resources\\Texture\\F1.png");

	shared_ptr<FMaterial> Material = make_shared<FMaterial>();
	Material->SetShader ( Shader );
	Material->SetMaterialParameters ( 0 , 0.3f );
	Material->SetMaterialParameters ( 1 , 0.4f );
	Material->SetMaterialParameters ( 2 , 0.3f );
	Material->SetTexture ( 0 , Texture );
	Mesh->SetMaterial ( Material );

	GEngine->GetCommandQueue()->WaitSync();
}

void Game::Update()
{

	// TEMP: 임시로 테스트를 위해 Begin, End 직접 호출
	GEngine->Update();

	GEngine->RenderBegin();	// Render는 Engine 내부에서 호출해줄 예정

	{
		// 초기값 0.5, 셰이더 파일에서 오프셋을 더해주고 있기 때문에 최종적으로 0.7 -> 뒤로 가짐
		static FTransform Transform{};
		if (GEngine->GetInput()->GetButton(EKeyCode::W))
		{
			Transform.Offset.y += 1.f * DELTA_TIME;
		}
		if ( GEngine->GetInput ( )->GetButton ( EKeyCode::S ) )
		{
			Transform.Offset.y -= 1.f * DELTA_TIME;
		}
		if ( GEngine->GetInput ( )->GetButton ( EKeyCode::A ) )
		{
			Transform.Offset.x -= 1.f * DELTA_TIME;
		}
		if ( GEngine->GetInput ( )->GetButton ( EKeyCode::D ) )
		{
			Transform.Offset.x += 1.f * DELTA_TIME;
		}

		Mesh->SetTransform(Transform);
		Mesh->Render();		// 내부적으로 Material->Update(), 그 속에서 다시 Shader->Update() 수행
	}

	GEngine->RenderEnd();
}
