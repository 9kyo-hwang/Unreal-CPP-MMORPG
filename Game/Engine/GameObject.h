#pragma once
#include "Component.h"

class MonoBehaviour;
class Transform;

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

	void Initialize();

	// 들고 있는 Component들의 Event Function들을 실행시킬 것임
	void Awake();
	void Start();
	void Update();
	void LateUpdate();

	shared_ptr<Transform> GetTransform() const;
	void AddComponent(shared_ptr<Component> Component);

private:
	array<shared_ptr<Component>, ComponentCount> Components;	// 컴포넌트는 유일하게 1개씩만
	vector<shared_ptr<MonoBehaviour>> Scripts;	// 스크립트는 동적으로 여러 개
};

