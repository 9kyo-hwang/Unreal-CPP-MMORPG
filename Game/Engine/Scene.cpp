#include "pch.h"
#include "Scene.h"

#include "GameObject.h"

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
