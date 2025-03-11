#pragma once

enum class EComponentType : uint8
{
	Transform,
	MeshRenderer,
	Camera,
	// ...
	MonoBehaviour,	// 항상 마지막에 위치
	END
};

constexpr uint8 ComponentCount = static_cast<uint8>(EComponentType::END) - 1;

class GameObject;
class Transform;

class Component
{
public:
	Component(EComponentType InType);
	virtual ~Component();

	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();	// 행렬 연산 등 엔진 단에서 활용할 최종 업데이트

	EComponentType GetType() const { return Type; }
	shared_ptr<GameObject> GetParent() const;
	shared_ptr<Transform> GetTransform() const;

private:
	friend class GameObject;
	void SetParent(const shared_ptr<GameObject>& InParent) { Parent = InParent; }

protected:
	EComponentType Type;
	weak_ptr<GameObject> Parent;	// Actor에서도 ActorComponent를 SharedPtr로 참조할 예정. 순환 참조를 막기 위해
};

