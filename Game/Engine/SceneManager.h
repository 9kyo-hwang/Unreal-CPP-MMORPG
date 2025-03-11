#pragma once

class Scene;

class SceneManager
{
	GENERATED_SINGLETON(SceneManager);

public:
	void Update();
	void Render();

	void LoadScene(wstring SceneName);

	shared_ptr<Scene> GetActiveScene() { return ActiveScene; }

private:
	shared_ptr<Scene> LoadTestScene();

private:
	shared_ptr<Scene> ActiveScene;
};

