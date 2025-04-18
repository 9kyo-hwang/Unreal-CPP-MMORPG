#include "pch.h"
#include "Scene.h"

#include "Camera.h"
#include "ConstantBuffer.h"
#include "Engine.h"
#include "GameObject.h"
#include "Light.h"
#include "Resources.h"

void Scene::Awake()
{
	// shared_ptr 복사가 일어나지 않도록 const&로 순회
	for (const shared_ptr<GameObject>& GameObject : GameObjects )
	{
		GameObject->Awake();
	}
}

void Scene::Start()
{
	for ( const shared_ptr<GameObject>& GameObject : GameObjects )
	{
		GameObject->Start();
	}
}

void Scene::Update()
{
	for ( const shared_ptr<GameObject>& GameObject : GameObjects )
	{
		GameObject->Update();
	}
}

void Scene::LateUpdate()
{
	for ( const shared_ptr<GameObject>& GameObject : GameObjects )
	{
		GameObject->LateUpdate();
	}
}

void Scene::FinalUpdate()
{
	for ( const shared_ptr<GameObject>& GameObject : GameObjects )
	{
		GameObject->FinalUpdate();
	}
}

void Scene::Render()
{
	PushLightData();	// 프레임 당 1번만

	// TODO: CommandQueue에서 수행하던 ClearRenderTargetView를 이곳에서 수행
	int8 BackBufferIndex = GEngine->GetSwapChain()->GetBackBufferIndex();

	// ClearRenderTargetView로 인해 STATE_COMMON -> STATE_RENDER_TARGET
	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::SwapChain)->ClearRenderTargetView(BackBufferIndex);
	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::GeometryBuffer)->ClearRenderTargetView();
	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::Lighting)->ClearRenderTargetView();

	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::GeometryBuffer)->OMSetRenderTargets();	// Deferred OMSet

	/**
	 *	메인 카메라에 대해서 우선적으로 연산 수행
	 *	Sort - Deferred - Light - Final - Forward
	 *	그리고 나머지 카메라에 대해 Sort - Forward 2단계만 수행
	 */
	auto MainCamera = Cameras[0];	// 0번 카메라를 메인 카메라로 가정
	MainCamera->SortGameObject();
	MainCamera->RenderDeferred();
	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::GeometryBuffer)->WaitForUseAsAResource();

	RenderLights();
	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::Lighting)->WaitForUseAsAResource();

	RenderFinal();
	MainCamera->RenderForward();

	for (auto& CameraComponent : Cameras)
	{
		if (CameraComponent == MainCamera) continue;

		CameraComponent->SortGameObject();
		CameraComponent->RenderForward();
	}
}

void Scene::RenderLights()
{
	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::Lighting)->OMSetRenderTargets();

	for (auto& LightComponent : Lights)
	{
		LightComponent->Render();
	}
}

void Scene::RenderFinal()
{
	// 모든 정보를 가지고 취합시키는 단계 -> BackBuffer 세팅, Final 데이터를 통해 Render

	int8 BackBufferIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::SwapChain)->OMSetRenderTargets(1, BackBufferIndex);

	Resources::Get()->Get<FMaterial>(L"Final")->PushGraphicsData();
	Resources::Get()->Get<FMesh>(L"Rectangle")->Render();
}

void Scene::AddGameObject(shared_ptr<GameObject> GameObject)
{
	if (auto CameraComponent = GameObject->GetCamera())
	{
		Cameras.push_back(CameraComponent);
	}
	else if (auto LightComponent = GameObject->GetLight())
	{
		Lights.push_back(LightComponent);
	}

	GameObjects.push_back(GameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> GameObject)
{
	if (auto CameraComponent = GameObject->GetCamera())
	{
		if (auto Iterator = ranges::find(Cameras, CameraComponent);
			Iterator != Cameras.end())
		{
			Cameras.erase(Iterator);
		}
	}
	else if (auto LightComponent = GameObject->GetLight())
	{
		if (auto Iterator = ranges::find(Lights, LightComponent);
			Iterator != Lights.end())
		{
			Lights.erase(Iterator);
		}
	}

	if (auto Iterator = ranges::find(GameObjects, GameObject);
		Iterator != GameObjects.end())
	{
		GameObjects.erase(Iterator);
	}
}

void Scene::PushLightData()
{
	FLightParameters Parameters{};
	for (auto& LightComponent : Lights)
	{
		LightComponent->SetIndex(Parameters.LightCount);	// Light를 Render할 때 Index를 통해 구분하기 위함
		Parameters.Lights[Parameters.LightCount++] = LightComponent->GetInfo();
	}

	CONSTANT_BUFFER(EConstantBufferType::Global)->SetGlobalGraphicsData(&Parameters, sizeof(Parameters));
}