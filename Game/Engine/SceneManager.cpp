#include "pch.h"
#include "SceneManager.h"

#include "CameraComponent.h"
#include "CameraMovementComponent.h"
#include "Engine.h"
#include "Actor.h"
#include "LightComponent.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "ParticleSystemComponent.h"
#include "Resources.h"
#include "Level.h"
#include "Shader.h"
#include "Texture.h"
#include "SceneComponent.h"

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

TSharedPtr<ULevel> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	// 기본값 레이어 설정
	SetLayer(0, L"Default");
	SetLayer(1, L"UI");
#pragma endregion

	// Compute Shader Texture를 UI에게 넘길 예정
#pragma region Compute Shader
	{
		TSharedPtr<FShader> Shader = Resources::Get()->Get<FShader>(L"ComputeShader");
		TSharedPtr<FTexture> Texture = Resources::Get()->CreateTexture(
			L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS	// NOT Render Target!
		);

		TSharedPtr<FMaterial> Material = Resources::Get()->Get<FMaterial>(L"ComputeShader");
		Material->SetShader(Shader);
		Material->SetParameter(0, 1);
		GEngine->GetComputeResourceTables()->SetUAV(Texture->GetUAV(), EUnorderedAccessViewRegisters::u0);

		Material->Dispatch(1, 256, 1);	// 쓰레드 그룹: (1, 1024, 1)
	}
#pragma endregion

	// 여기서 임시로 만든 신을 ActiveScene으로 지정
	TSharedPtr<ULevel> CurrentScene = MakeShared<ULevel>();

#pragma region Camera
	{
		TSharedPtr<AActor> CameraObject = MakeShared<AActor>();
		CameraObject->SetName(L"Main Camera");

		CameraObject->AddComponent(MakeShared<USceneComponent>());
		CameraObject->AddComponent(MakeShared<UCameraComponent>());
		CameraObject->AddComponent(MakeShared<UCameraMovementComponent>());
		CameraObject->GetSceneComponent()->SetLocalPosition(FVector3(0.f, 0.f, 0.f));

		uint8 Layer = Get()->NameToLayer(L"UI");
		CameraObject->GetCameraComponent()->EnableLayerCulling(Layer, true);	// UI는 그리지 않도록

		CurrentScene->SpawnActor(CameraObject);
	}
#pragma endregion

#pragma region UI Camera
	{
		TSharedPtr<AActor> CameraObject = MakeShared<AActor>();
		CameraObject->SetName(L"Orthographic Camera");

		CameraObject->AddComponent(MakeShared<USceneComponent>());
		CameraObject->AddComponent(MakeShared<UCameraComponent>());	// Near: 1, Far: 1000, Fov: None, 800 x 600
		CameraObject->GetSceneComponent()->SetLocalPosition(FVector3(0.f, 0.f, 0.f));

		auto CameraComponent = CameraObject->GetCameraComponent();
		uint8 Layer = Get()->NameToLayer(L"UI");

		CameraComponent->SetProjection(ECameraProjectionType::Orthographic);
		CameraComponent->CullAllLayers();
		CameraComponent->EnableLayerCulling(Layer, false);	// UI만 그리도록

		CurrentScene->SpawnActor(CameraObject);
	}
#pragma endregion

#pragma region Skybox
	{
		TSharedPtr<AActor> SkyboxObject = MakeShared<AActor>();
		SkyboxObject->AddComponent(MakeShared<USceneComponent>());	// 크기, 위치, 회전 아무것도 설정하지 않음
		SkyboxObject->SetCheckFrustum(false);	// Skybox는 예외적으로 Frustum 범위 밖에 있더라도 그려져야 함

		TSharedPtr<UMeshComponent> MeshRenderer = MakeShared<UMeshComponent>();
		MeshRenderer->SetMesh(Resources::Get()->LoadSphere());

		TSharedPtr<FShader> Shader = Resources::Get()->Get<FShader>(L"Skybox");
		TSharedPtr<FTexture> Texture = Resources::Get()->Load<FTexture>(L"Sky02", L"..\\Resources\\Texture\\Sky02.jpeg");

		TSharedPtr<FMaterial> Material = MakeShared<FMaterial>();
		Material->SetShader(Shader);
		Material->SetTexture(0, Texture);

		MeshRenderer->SetMaterial(Material);
		SkyboxObject->AddComponent(MeshRenderer);
		CurrentScene->SpawnActor(SkyboxObject);
	}
#pragma endregion

#pragma region Object
	for(int32 Index = 0; Index < 50; ++Index)
	{
		TSharedPtr<AActor> Actor = MakeShared<AActor>();
		Actor->AddComponent(MakeShared<USceneComponent>());
		Actor->GetSceneComponent()->SetLocalScale(FVector3(25.f, 25.f, 25.f));
		Actor->GetSceneComponent()->SetLocalPosition(FVector3(-300.f + Index * 10.f, 0.f, 500.f));

		TSharedPtr<UMeshComponent> MeshRenderer = MakeShared<UMeshComponent>();
		MeshRenderer->SetMesh(Resources::Get()->LoadSphere());

		// Instance를 활용하려면 Shader, Texture 등이 모두 "같은 것"이어야 함 -> Resources에서 한 번만 생성하도록
		TSharedPtr<FMaterial> Material = Resources::Get()->Get<FMaterial>(L"GameObject");
		{	// 기존
			//Material->SetParameter(0, 0);
			//MeshRenderer->SetMaterial(Material->Clone());	// 별개의 Material로 인식하도록 복사
		}
		{	// 인스턴스
			Material->SetParameter(0, 1);	// value 1: Instance <-> 0: 기존
			MeshRenderer->SetMaterial(Material);
		}
		
		Actor->AddComponent(MeshRenderer);
		CurrentScene->SpawnActor(Actor);
	}
#pragma endregion

#pragma region UI Test
	for (int32 Index = 0; Index < 6; ++Index)
	{
		TSharedPtr<AActor> UIObject = MakeShared<AActor>();
		UIObject->SetLayer(Get()->NameToLayer(L"UI"));	// UI, 즉 1번으로 세팅
		UIObject->AddComponent(MakeShared<USceneComponent>());

		TSharedPtr<USceneComponent> Transform = UIObject->GetSceneComponent();
		Transform->SetLocalScale(FVector3(100.f, 100.f, 100.f));
		Transform->SetLocalPosition(FVector3(-350.f + (Index * 120), 250.f, 500.f));

		TSharedPtr<UMeshComponent> MeshRenderer = MakeShared<UMeshComponent>();
		MeshRenderer->SetMesh(Resources::Get()->LoadRectangle());

		TSharedPtr<FShader> Shader = Resources::Get()->Get<FShader>(L"Texture");
		TSharedPtr<FTexture> Texture;
		if (Index < 3)
		{
			Texture = GEngine->GetMultipleRenderTarget(ERenderTargetType::GeometryBuffer)->GetRenderTargetTexture(Index);
		}
		else if (Index < 5)
		{
			Texture = GEngine->GetMultipleRenderTarget(ERenderTargetType::Lighting)->GetRenderTargetTexture(Index - 3);
		}
		else
		{
			Texture = Resources::Get()->Get<FTexture>(L"UAVTexture");
		}

		auto Material = MakeShared<FMaterial>();
		Material->SetShader(Shader);
		Material->SetTexture(0, Texture);

		MeshRenderer->SetMaterial(Material);
		UIObject->AddComponent(MeshRenderer);
		CurrentScene->SpawnActor(UIObject);
	}
#pragma endregion

#pragma region Directional Light
	{
		TSharedPtr<AActor> LightObject = MakeShared<AActor>();
		LightObject->AddComponent(MakeShared<USceneComponent>());
		LightObject->AddComponent(MakeShared<ULightComponent>());

		TSharedPtr<ULightComponent> LightComponent = LightObject->GetLightComponent();
		LightComponent->SetDirection(FVector3(0.f, 0.f, 1.f));
		LightComponent->SetType(ELightType::Directional);
		LightComponent->SetDiffuse(FVector3(1.f, 1.f, 1.f));
		LightComponent->SetAmbient(FVector3(0.1f, 0.1f, 0.1f));
		LightComponent->SetSpecular(FVector3(0.2f, 0.2f, 0.2f));

		CurrentScene->SpawnActor(LightObject);
	}
#pragma endregion

	

	return CurrentScene;
}
