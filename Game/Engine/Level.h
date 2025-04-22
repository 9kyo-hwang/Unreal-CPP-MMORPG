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

	void SpawnActor(TSharedPtr<AActor> Actor);
	void RemoveActor(TSharedPtr<AActor> Actor);

	const vector<TSharedPtr<AActor>>& GetAllLevelActors() { return Actors; }

private:
	void PushLightData();
	void ClearRTV();
	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();
	void RenderForward();

private:
	// TODO: Layer 단계를 추가해 최적화

	vector<TSharedPtr<AActor>> Actors;
	vector<TSharedPtr<class UCameraComponent>> Cameras;
	vector<TSharedPtr<class ULightComponent>> Lights;
};
