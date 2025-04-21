#pragma once

class AActor;

class ULevel
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

	void SpawnActor(TSharedPtr<AActor> Actor);
	void RemoveActor(TSharedPtr<AActor> Actor);

	const vector<TSharedPtr<AActor>>& GetAllActor() { return Actors; }

private:
	void PushLightData();

private:
	// TODO: Layer 단계를 추가해 최적화

	vector<TSharedPtr<AActor>> Actors;
	vector<TSharedPtr<class UCameraComponent>> Cameras;
	vector<TSharedPtr<class ULightComponent>> Lights;
};
