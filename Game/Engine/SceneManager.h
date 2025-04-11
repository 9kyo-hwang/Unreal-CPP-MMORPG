#pragma once

constexpr int32 NumLayer = 32;

class Scene;

class SceneManager
{
	GENERATED_SINGLETON(SceneManager);

public:
	void Update();
	void Render();

	void LoadScene(wstring SceneName);

	shared_ptr<Scene> GetActiveScene() { return ActiveScene; }

	void SetLayer(uint8 Layer, const wstring& Name);
	const wstring& LayerToName(uint8 Index) { return LayerNames[Index]; }
	uint8 NameToLayer(const wstring& Name);

private:
	shared_ptr<Scene> LoadTestScene();

private:
	shared_ptr<Scene> ActiveScene;

	array<wstring, NumLayer> LayerNames;	// Client 측에서 string을 통해 자유롭게 추가/삭제 가능하도록 string으로 관리
	unordered_map<wstring, uint8> Layers;
};

