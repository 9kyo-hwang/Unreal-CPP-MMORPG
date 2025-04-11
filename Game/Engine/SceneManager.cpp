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

void SceneManager::SetLayer(uint8 Layer, const wstring& Name)
{
	const wstring& PrevLayer = LayerNames[Layer];
	Layers.erase(PrevLayer);

	LayerNames[Layer] = Name;
	Layers[Name] = Layer;
}

uint8 SceneManager::NameToLayer(const wstring& Name)
{
	if (Layers.contains(Name))
	{
		return Layers[Name];
	}

	return 0;
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	// 기본값 레이어 설정
	SetLayer(0, L"Default");
	SetLayer(1, L"UI");
#pragma endregion

	// 여기서 임시로 만든 신을 ActiveScene으로 지정
	shared_ptr<Scene> CurrentScene = make_shared<Scene>();

#pragma region Camera
	{
		shared_ptr<GameObject> CameraObject = make_shared<GameObject>();
		CameraObject->SetName(L"Perspective Camera");

		CameraObject->AddComponent(make_shared<Transform>());
		CameraObject->AddComponent(make_shared<Camera>());
		CameraObject->AddComponent(make_shared<CameraMovementComponent>());
		CameraObject->GetTransform()->SetLocalPosition(FVector3(0.f, 0.f, 0.f));

		uint8 Layer = SceneManager::Get()->NameToLayer(L"UI");
		CameraObject->GetCamera()->EnableLayerCulling(Layer, true);	// UI는 그리지 않도록

		CurrentScene->AddGameObject(CameraObject);
	}
#pragma endregion

#pragma region UI Camera
	{
		shared_ptr<GameObject> CameraObject = make_shared<GameObject>();
		CameraObject->SetName(L"Orthographic Camera");

		CameraObject->AddComponent(make_shared<Transform>());
		CameraObject->AddComponent(make_shared<Camera>());	// Near: 1, Far: 1000, Fov: None, 800 x 600

		CameraObject->GetTransform()->SetLocalPosition(FVector3(0.f, 0.f, 0.f));

		auto Camera = CameraObject->GetCamera();
		uint8 Layer = SceneManager::Get()->NameToLayer(L"UI");

		Camera->SetProjection(ECameraProjectionType::Orthographic);
		Camera->CullAllLayers();
		Camera->EnableLayerCulling(Layer, false);	// UI만 그리도록

		CurrentScene->AddGameObject(CameraObject);
	}
#pragma endregion

#pragma region Skybox
	{
		shared_ptr<GameObject> SkyboxObject = make_shared<GameObject>();
		SkyboxObject->AddComponent(make_shared<Transform>());	// 크기, 위치, 회전 아무것도 설정하지 않음
		SkyboxObject->SetCheckFrustum(false);	// Skybox는 예외적으로 Frustum 범위 밖에 있더라도 그려져야 함

		shared_ptr<FMeshRenderer> MeshRenderer = make_shared<FMeshRenderer>();
		{
			shared_ptr<FMesh> Skybox = Resources::Get()->LoadSphere();
			MeshRenderer->SetMesh(Skybox);
		}
		{
			shared_ptr<FShader> Shader = Resources::Get()->Get<FShader>(L"Skybox");
			shared_ptr<FTexture> Texture = Resources::Get()->Load<FTexture>(L"Sky02", L"..\\Resources\\Texture\\Sky02.jpeg");

			shared_ptr<FMaterial> Material = make_shared<FMaterial>();
			Material->SetShader(Shader);
			Material->SetTexture(0, Texture);

			MeshRenderer->SetMaterial(Material);
		}

		SkyboxObject->AddComponent(MeshRenderer);
		CurrentScene->AddGameObject(SkyboxObject);
	}
#pragma endregion

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
			shared_ptr<FShader> Shader = Resources::Get()->Get<FShader>(L"Forward");
			shared_ptr<FTexture> Texture = Resources::Get()->Load<FTexture>(L"Wood", L"..\\Resources\\Texture\\Wood.jpg");
			shared_ptr<FTexture> NormalTexture =  Resources::Get()->Load<FTexture>(L"Wood_Normal", L"..\\Resources\\Texture\\Wood_Normal.jpg");

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

#pragma region UI Test
	{
		shared_ptr<GameObject> SphereObject = make_shared<GameObject>();
		SphereObject->SetLayer(SceneManager::Get()->NameToLayer(L"UI"));	// UI, 즉 1번으로 세팅
		SphereObject->AddComponent(make_shared<Transform>());

		auto Transform = SphereObject->GetTransform();
		Transform->SetLocalScale(FVector3(100.f, 100.f, 100.f));
		Transform->SetLocalPosition(FVector3(0, 0, 500.f));

		shared_ptr<FMeshRenderer> MeshRenderer = make_shared<FMeshRenderer>();
		{
			MeshRenderer->SetMesh(Resources::Get()->LoadRectangle());
		}
		{
			auto Shader = Resources::Get()->Get<FShader>(L"Forward");
			auto Texture = Resources::Get()->Load<FTexture>(L"Wood", L"..\\Resources\\Texture\\Wood.jpg");

			auto Material = make_shared<FMaterial>();
			Material->SetShader(Shader);
			Material->SetTexture(0, Texture);
			MeshRenderer->SetMaterial(Material);
		}

		SphereObject->AddComponent(MeshRenderer);
		CurrentScene->AddGameObject(SphereObject);
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

	return CurrentScene;
}
