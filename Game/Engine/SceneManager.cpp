#include "pch.h"
#include "SceneManager.h"

#include "Camera.h"
#include "CameraMovement.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

void SceneManager::Update()
{
	if (!ActiveScene)
	{
		return;
	}

	ActiveScene->Update();
	ActiveScene->LateUpdate();
	ActiveScene->FinalUpdate();
}

// TEMP
void SceneManager::Render()
{
	if (!ActiveScene )
	{
		return;
	}

	for (const auto& GameObject : GetActiveScene()->GetGameObjects())
	{
		if (const auto& Camera = GameObject->GetCamera())
		{
			Camera->Render();
		}
	}
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
	// 여기서 임시로 만든 신을 ActiveScene으로 지정
	shared_ptr<Scene> CurrentScene = make_shared<Scene>();

#pragma region Camera
	shared_ptr<GameObject> CameraObject = make_shared<GameObject>();
	CameraObject->AddComponent(make_shared<Transform>());
	CameraObject->AddComponent(make_shared<Camera>());
	CameraObject->AddComponent(make_shared<CameraMovementComponent>());
	CameraObject->GetTransform()->SetLocalPosition(FVector3(0.f, 100.f, 0.f));

	CurrentScene->AddGameObject(CameraObject);
#pragma endregion

#pragma region Sphere
	{
		shared_ptr<GameObject> SphereObject = make_shared<GameObject>();
		SphereObject->AddComponent(make_shared<Transform>());
		SphereObject->GetTransform()->SetLocalScale(FVector3(100.f, 100.f, 100.f));
		SphereObject->GetTransform()->SetLocalPosition(FVector3(0.f, 100.f, 200.f));

		shared_ptr<FMeshRenderer> MeshRenderer = make_shared<FMeshRenderer>();
		{
			shared_ptr<FMesh> Sphere = Resources::Get()->LoadSphere();
			MeshRenderer->SetMesh(Sphere);
		}
		{
			shared_ptr<FShader> Shader = make_shared<FShader>();
			shared_ptr<FTexture> Texture = make_shared<FTexture>();
			Shader->Initialize(L"..\\Resources\\Shader\\Default.hlsli");
			Texture->Initialize(L"..\\Resources\\Texture\\F1.png");

			shared_ptr<FMaterial> Material = make_shared<FMaterial>();
			Material->SetShader(Shader);
			Material->SetTexture(0, Texture);

			MeshRenderer->SetMaterial(Material);
		}

		SphereObject->AddComponent(MeshRenderer);
		CurrentScene->AddGameObject(SphereObject);
	}
#pragma endregion

#pragma region Cube
	{
		shared_ptr<GameObject> CubeObject = make_shared<GameObject>();
		CubeObject->AddComponent(make_shared<Transform>());
		CubeObject->GetTransform()->SetLocalScale(FVector3(100.f, 100.f, 100.f));
		CubeObject->GetTransform()->SetLocalPosition(FVector3(150.f, 100.f, 200.f));

		shared_ptr<FMeshRenderer> MeshRenderer = make_shared<FMeshRenderer>();
		{
			shared_ptr<FMesh> Sphere = Resources::Get()->LoadCube();
			MeshRenderer->SetMesh(Sphere);
		}
		{
			shared_ptr<FShader> Shader = make_shared<FShader>();
			shared_ptr<FTexture> Texture = make_shared<FTexture>();
			Shader->Initialize(L"..\\Resources\\Shader\\Default.hlsli");
			Texture->Initialize(L"..\\Resources\\Texture\\F1.png");

			shared_ptr<FMaterial> Material = make_shared<FMaterial>();
			Material->SetShader(Shader);
			Material->SetTexture(0, Texture);

			MeshRenderer->SetMaterial(Material);
		}

		CubeObject->AddComponent(MeshRenderer);
		CurrentScene->AddGameObject(CubeObject);
	}
#pragma endregion
	
	return CurrentScene;
}
