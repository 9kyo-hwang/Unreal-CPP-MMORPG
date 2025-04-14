#include "pch.h"
#include "Scene.h"

#include "Camera.h"
#include "ConstantBuffer.h"
#include "Engine.h"
#include "GameObject.h"
#include "Light.h"

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
	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::SwapChain)->ClearRenderTargetView(BackBufferIndex);
	GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::GeometryBuffer)->ClearRenderTargetView();

	for (const auto& GameObject : GameObjects)
	{
		if (const auto& Camera = GameObject->GetCamera())
		{
			/**
			 *	1. Camera에 표시할 게임 오브젝트들을 정렬
			 *	2. Deferred 셰이더들에 대한 OMSetRenderTargets 수행
			 *	3. Light에 대한 OMSet 수행
			 *	4. SwapChain에 대한 OMSet 수행
			 */

			Camera->SortGameObject();

			GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::GeometryBuffer)->OMSetRenderTargets();
			Camera->RenderDeferred();

			GEngine->GetMultipleRenderTarget(EMultipleRenderTargetType::SwapChain)->OMSetRenderTargets(1, BackBufferIndex);
			Camera->RenderForward();
		}
	}
}

void Scene::AddGameObject(shared_ptr<GameObject> GameObject)
{
	GameObjects.push_back(GameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> GameObject)
{
	if (auto Iterator = ranges::find(GameObjects, GameObject);
		Iterator != GameObjects.end())
	{
		GameObjects.erase(Iterator);
	}
}

void Scene::PushLightData()
{
	FLightParameters Parameters{};
	for (auto& GameObject : GameObjects)
	{
		if ( const auto& LightComponent = GameObject->GetLight() )
		{
			Parameters.Lights[Parameters.LightCount++] = LightComponent->GetInfo();
		}
	}

	CONSTANT_BUFFER(EConstantBufferType::Global)->SetStaticData(&Parameters, sizeof(Parameters));
}