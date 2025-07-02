#include "pch.h"
#include <thread>
#include "CoreGlobal.h"
#include "RefCountBase.h"
#include "ThreadManager.h"

class FWraight : public FRefCountBase
{
public:
	int32 Hp = 0;
};

class FMissile : public FRefCountBase
{
public:
	void SetTarget(TSharedPtr<FWraight> InTarget)
	{
		Target = InTarget;
		Target->AddRef();
	}

	void Update()
	{
		if (Target == nullptr)
		{
			return;
		}

		// TODO: Chase

		if (Target->Hp == 0)
		{
			Target->Release();
			Target = nullptr;
		}
	}

private:
	TSharedPtr<FWraight> Target = nullptr;
};

int main()
{
	TSharedPtr<FWraight> Wraight(new FWraight());
	// RefCountBase에서 카운트 1로 시작, 셰어드 포인터를 생성할 때 Set 함수 호출로 인해 카운트 1 증가
	// 따라서 최초에 한해 1번 Release를 명시적으로 호출해줘야 함
	Wraight->Release();

	TSharedPtr<FMissile> Missile(new FMissile());
	Missile->Release();

	// 복사되어 전달, 복사 생성자 동작으로 인해 참조 카운트 증가
	Missile->SetTarget(Wraight);

	return 0;
}
