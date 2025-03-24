#include "pch.h"
#include "SceneManager.h"

#include "Camera.h"
#include "CameraMovement.h"
#include "GameObject.h"
#include "Light.h"
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
	if (ActiveScene )
	{
		ActiveScene->Render();
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
	CameraObject->GetTransform()->SetLocalPosition(FVector3(0.f, 0.f, 0.f));

	CurrentScene->AddGameObject(CameraObject);
#pragma endregion

//#pragma region Sphere
//	{
//		shared_ptr<GameObject> SphereObject = make_shared<GameObject>();
//		SphereObject->AddComponent(make_shared<Transform>());
//		SphereObject->GetTransform()->SetLocalScale(FVector3(100.f, 100.f, 100.f));
//		SphereObject->GetTransform()->SetLocalPosition(FVector3(0.f, 0.f, 150.f));
//
//		shared_ptr<FMeshRenderer> MeshRenderer = make_shared<FMeshRenderer>();
//		{
//			shared_ptr<FMesh> Sphere = Resources::Get()->LoadSphere();
//			MeshRenderer->SetMesh(Sphere);
//		}
//		{
//			shared_ptr<FShader> Shader = make_shared<FShader>();
//			shared_ptr<FTexture> Texture = make_shared<FTexture>();
//			Shader->Initialize(L"..\\Resources\\Shader\\Default.hlsli");
//			Texture->Initialize(L"..\\Resources\\Texture\\F1.png");
//
//			shared_ptr<FMaterial> Material = make_shared<FMaterial>();
//			Material->SetShader(Shader);
//			Material->SetTexture(0, Texture);
//
//			MeshRenderer->SetMaterial(Material);
//		}
//
//		SphereObject->AddComponent(MeshRenderer);
//		CurrentScene->AddGameObject(SphereObject);
//	}
//#pragma endregion

#pragma region Cube
	{
		shared_ptr<GameObject> CubeObject = make_shared<GameObject>();
		CubeObject->AddComponent(make_shared<Transform>());
		CubeObject->GetTransform()->SetLocalScale(FVector3(100.f, 100.f, 100.f));
		CubeObject->GetTransform()->SetLocalPosition(FVector3(0.f, 0.f, 150.f));

		shared_ptr<FMeshRenderer> MeshRenderer = make_shared<FMeshRenderer>();
		{
			shared_ptr<FMesh> Sphere = Resources::Get()->LoadCube();
			MeshRenderer->SetMesh(Sphere);
		}
		{
			shared_ptr<FShader> Shader = make_shared<FShader>();
			shared_ptr<FTexture> Texture = make_shared<FTexture>();
			shared_ptr<FTexture> NormalTexture = make_shared<FTexture>();
			Shader->Initialize(L"..\\Resources\\Shader\\Default.hlsli");
			Texture->Initialize(L"..\\Resources\\Texture\\Wood.jpg");
			NormalTexture->Initialize(L"..\\Resources\\Texture\\Wood_Normal.jpg");

			shared_ptr<FMaterial> Material = make_shared<FMaterial>();
			Material->SetShader(Shader);
			Material->SetTexture(0, Texture);
			Material->SetTexture(1, NormalTexture);

			MeshRenderer->SetMaterial(Material);
		}

		CubeObject->AddComponent(MeshRenderer);
		CurrentScene->AddGameObject(CubeObject);
	}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> LightObject = make_shared<GameObject>();
		LightObject->AddComponent(make_shared<Transform>());

		LightObject->AddComponent(make_shared<Light>());

		shared_ptr<Light> LightComponent = LightObject->GetLight();
		LightComponent->SetDirection(FVector3(1.f, 0.f, 1.f));
		LightComponent->SetType(ELightType::Directional);
		LightComponent->SetDiffuse(FVector3(0.5f, 0.5f, 0.5f));
		LightComponent->SetAmbient(FVector3(0.1f, 0.1f, 0.1f));
		LightComponent->SetSpecular(FVector3(0.3f, 0.3f, 0.3f));

		CurrentScene->AddGameObject(LightObject);
	}
#pragma endregion

//#pragma region Red Point Light
//	{
//		shared_ptr<GameObject> LightObject = make_shared<GameObject>();
//		LightObject->AddComponent(make_shared<Transform>());
//		LightObject->GetTransform()->SetLocalPosition(FVector3(150.f, 150.f, 150.f));
//
//		LightObject->AddComponent(make_shared<Light>());
//		shared_ptr<Light> LightComponent = LightObject->GetLight();
//		LightComponent->SetType(ELightType::Point);
//		LightComponent->SetDiffuse(FVector3(1.f, 0.1f, 0.1f));
//		LightComponent->SetAmbient(FVector3(0.1f, 0.f, 0.f));
//		LightComponent->SetSpecular(FVector3(0.1f, 0.1f, 0.1f));
//		LightComponent->SetRange(10000.f);
//
//		CurrentScene->AddGameObject(LightObject);
//	}
//#pragma endregion
//
//#pragma region Blue Spot Light
//	{
//		shared_ptr<GameObject> LightObject = make_shared<GameObject>();
//		LightObject->AddComponent(make_shared<Transform>());
//		LightObject->GetTransform()->SetLocalPosition(FVector3(-150.f, 0.f, 150.f));
//
//		LightObject->AddComponent(make_shared<Light>());
//
//		shared_ptr<Light> LightComponent = LightObject->GetLight();
//		LightComponent->SetDirection(FVector3(1.f, 0.f, 0.f));
//		LightComponent->SetType(ELightType::Spot);
//		LightComponent->SetDiffuse(FVector3(0.f, 0.1f, 1.f));
//		LightComponent->SetSpecular(FVector3(0.1f, 0.1f, 0.1f));
//		LightComponent->SetRange(10000.f);
//		LightComponent->SetAngle(XM_PI / 4);
//
//		CurrentScene->AddGameObject(LightObject);
//	}
//#pragma endregion

	return CurrentScene;
}
