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

	for (const auto& GameObject : GameObjects)
	{
		if (const auto& Camera = GameObject->GetCamera())
		{
			Camera->Render();
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