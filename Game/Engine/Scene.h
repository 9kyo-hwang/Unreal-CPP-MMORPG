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

	void Render();
	void RenderLights();
	void RenderFinal();

	void AddGameObject(shared_ptr<GameObject> GameObject);
	void RemoveGameObject(shared_ptr<GameObject> GameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return GameObjects; }

private:
	void PushLightData();

private:
	// TODO: Layer 단계를 추가해 최적화

	vector<shared_ptr<GameObject>> GameObjects;
	vector<shared_ptr<class Camera>> Cameras;
	vector<shared_ptr<class Light>> Lights;
};
