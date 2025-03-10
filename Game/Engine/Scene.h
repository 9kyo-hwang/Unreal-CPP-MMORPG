#pragma once

class GameObject;

class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();

	void AddGameObject(shared_ptr<GameObject> GameObject);
	void RemoveGameObject(shared_ptr<GameObject> GameObject);

private:
	vector<shared_ptr<GameObject>> GameObjects;

	// TODO: Layer 단계를 추가해 최적화
};
