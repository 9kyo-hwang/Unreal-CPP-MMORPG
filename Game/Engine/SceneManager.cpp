#include "pch.h"
#include "SceneManager.h"

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"

void SceneManager::Update()
{
	if (!ActiveScene)
	{
		return;
	}

	ActiveScene->Update();
	ActiveScene->LateUpdate();
}

void SceneManager::LoadScene(wstring SceneName)
{
	// TODO: 기존 씬 정리
	// TODO: 파일에서 Scene 정보 로드

	ActiveScene = LoadTestScene();
	ActiveScene->Awake();
	ActiveScene->Start();
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
	// 사각형은 삼각형 2개를 이용해 그리는 것이므로 정점 정보가 6개 있어야 함
	vector<FVertex> Vertices
	{
		{
			FVector3(-0.5f , 0.5f , 0.5f),
			FVector4(1.f , 0.f , 0.f , 1.f),
			FVector2(0.f , 0.f)
		},
		{
			FVector3(0.5f , 0.5f , 0.5f),
			FVector4(0.f , 1.f , 0.f , 1.f),
			FVector2(1.f , 0.f)
		},
		{
			FVector3(0.5f , -0.5f , 0.5f),
			FVector4(0.f , 0.f , 1.f , 1.f),
			FVector2(1.f , 1.f)
		},
		{
			FVector3(-0.5f , -0.5f , 0.5f),
			FVector4(0.f , 1.f , 0.f , 1.f),
			FVector2(0.f , 1.f)
		}
	};

	vector<uint32> Indices{ 0, 1, 2, 0, 2, 3 };

	shared_ptr<GameObject> Object = make_shared<GameObject>();
	Object->Initialize();	// Add Transform Component

	// Test
	shared_ptr<FMeshRenderer> MeshRenderer = make_shared<FMeshRenderer>();
	{
		shared_ptr<FMesh> Mesh = make_shared<FMesh>();
		Mesh->Initialize(Vertices, Indices);
		MeshRenderer->SetMesh(Mesh);
	}
	{
		shared_ptr<FShader> Shader = make_shared<FShader>();
		shared_ptr<FTexture> Texture = make_shared<FTexture>();
		Shader->Initialize(L"..\\Resources\\Shader\\Default.hlsli");
		Texture->Initialize(L"..\\Resources\\Texture\\F1.png");

		shared_ptr<FMaterial> Material = make_shared<FMaterial>();
		Material->SetShader(Shader);
		Material->SetMaterialParameters(0, 0.3f);
		Material->SetMaterialParameters(1, 0.4f);
		Material->SetMaterialParameters(2, 0.3f);
		Material->SetTexture(0, Texture);

		MeshRenderer->SetMaterial(Material);
	}

	Object->AddComponent(MeshRenderer);

	// 여기서 임시로 만든 신을 ActiveScene으로 지정
	shared_ptr<Scene> CurrentScene = make_shared<Scene>();
	CurrentScene->AddGameObject(Object);
	return CurrentScene;
}
