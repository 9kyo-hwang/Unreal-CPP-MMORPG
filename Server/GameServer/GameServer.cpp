#include "pch.h"
#include <thread>
#include "CoreGlobal.h"
#include "Allocator.h"
#include "Casts.h"
#include "Memory.h"
#include "MemoryBase.h"
#include "RefCountBase.h"
#include "ThreadManager.h"

using ActorList = TypeList<class Player, class Knight, class Monster>;

class Player
{
public:
	Player()
	{
		TypeId = IndexOf<Types, Player>::Value;
	}

	virtual ~Player() {}

	using Types = ActorList;
	int32 TypeId;

private:
};

class Knight : public Player
{
public:
	Knight()
	{
		TypeId = IndexOf<Types, Knight>::Value;
	}
	~Knight() override {}

	int32 Hp = rand() % 1000;
};

class Monster
{
public:
	Monster()
	{
		TypeId = IndexOf<Types, Monster>::Value;
	}

	~Monster() {}

	using Types = ActorList;
	int32 TypeId;
};

int main()
{
	//TypeList<Knight>::First SingleHead;
	//TypeList<Knight, Monster>::First PairHead;
	//TypeList<Knight, Monster>::Second PairTail;
	//TypeList<Knight, TypeList<Monster, Monster>>::First DPHead;
	//TypeList<Knight, TypeList<Monster, Monster>>::Second::First DPTailHead;
	//TypeList<Knight, TypeList<Monster, Monster>>::Second::First DPTailTail;

	//int64 Len = SizeOf<TypeList<Knight, Monster, Monster>>::Value;
	//TypeAt<TypeList<Knight, Monster, Knight>, 2>::Value Type2;
	//int64 Index = IndexOf<TypeList<Knight, Monster>, Knight>::Value;

	//bool canCastFromPlayerToKnight = Convert<Player, Knight>::Result;
	//bool canCastFromKnightToPlayer = Convert<Knight, Player>::Result;
	//bool canCastFromPlayerToMonster = Convert<Player, Monster>::Result;

	{
		shared_ptr<Knight> K = MakeShared<Knight>();
		shared_ptr<Player> P = Cast<Player>(K);
		bool CanCast = IsA<Player>(K);
	}

	return 0;
}
