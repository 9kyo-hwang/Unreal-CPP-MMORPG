#pragma once
#include "Component.h"
#include "Object.h"

class Light;
class Camera;
class FMeshRenderer;
class MonoBehaviour;
class Transform;

class GameObject : public Object, public enable_shared_from_this<GameObject>
{
	using Super = Object;

public:
	GameObject();
	~GameObject() override;

	// 들고 있는 Component들의 Event Function들을 실행시킬 것임
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	shared_ptr<Component> GetComponent(EComponentType Type) const;
	void AddComponent(shared_ptr<Component> Component);

public:
	shared_ptr<Transform> GetTransform() const;
	shared_ptr<FMeshRenderer> GetMeshRenderer() const;
	shared_ptr<Camera> GetCamera() const;
	shared_ptr<Light> GetLight() const;

private:
	array<shared_ptr<Component>, ComponentCount> Components;	// 컴포넌트는 유일하게 1개씩만
	vector<shared_ptr<MonoBehaviour>> Scripts;	// 스크립트는 동적으로 여러 개
};

