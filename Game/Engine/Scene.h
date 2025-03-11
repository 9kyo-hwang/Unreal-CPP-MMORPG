#pragma once

class GameObject;

class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	void AddGameObject(shared_ptr<GameObject> GameObject);
	void RemoveGameObject(shared_ptr<GameObject> GameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return GameObjects; }

private:
	vector<shared_ptr<GameObject>> GameObjects;

	// TODO: Layer 단계를 추가해 최적화
};
