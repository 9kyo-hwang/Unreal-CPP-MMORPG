#pragma once

#include <mutex>

template<typename T>
class TStack
{
public:
	TStack() {}
	TStack(const TStack&) = delete;
	TStack& operator=(const TStack&) = delete;

	void Emplace(const T& InValue)
	{
		FRefCountedObject Node;
		Node.Ptr = new FNode(InValue);
		Node.NumExternalRefs = 1;

		// CAS
		Node.Ptr->Next = Head;
		while (Head.compare_exchange_weak(Node.Ptr->Next, Node) == false)
		{
			
		}
	}

	shared_ptr<T> Pop()
	{
		FRefCountedObject Prev = Head;
		while (true)
		{
			// 참조 횟수를 1 증가시킨 스레드는 해당 데이터에 접근할 수 있는 권한이 주어짐
			IncreaseNumRef(Prev);

			FNode* Ptr = Prev.Ptr;
			if (Ptr == nullptr)
			{
				return shared_ptr<T>();
			}

			// Ptr(데이터)->Next로 Head를 변경한 스레드는 해당 데이터를 사용할 수 있는 권한이 주어짐
			if (Head.compare_exchange_strong(Prev, Ptr->Next))
			{
				// 위에서 참조 횟수 1 증가시킨 데이터라면 이곳에 진입
				// 다른 스레드가 참조 횟수를 증가시켰다면 Prev != Head(참조 횟수가 다름)
				// 일종의 번호표 개념(나: 1 -> 2 / 다른 스레드: 2 -> 3)

				shared_ptr<T> Resource;
				Resource.swap(Ptr->Data);

				/**
				 *	FRefCountedObject의 NumExternalRefs 기본값 1, 자신의 스레드만 있다면 값이 2로 세팅돼있음(순증가)
				 *	FNode의 NumRefs는 기본값 0(순감소)
				 */

				// 다른 스레드가 이 데이터에 접근중이면 삭제하면 안됨! 이를 확인
				const int32 NumRefsIncreased = Prev.NumExternalRefs - 2;	// 나밖에 없었다면 2 - 0 = 0

				// 즉 아래 수식은 {NumInternalRefs(0) + 0} 후 0(연산 이전 값) == -0 -> 스레드는 나 밖에 없음
				// 나 외에 다른 스레드가 있다면 {0 + 2(예시)} 후 0 == -2 -> 다른 스레드가 참조 중이라 삭제하면 안됨!
				if (Ptr->NumInternalRefs.fetch_add(NumRefsIncreased) == -NumRefsIncreased)
				{
					delete Ptr;
				}

				return Resource;
			}
			else if (Ptr->NumInternalRefs.fetch_sub(1) == 1)
			{
				// 데이터 접근엔 성공했으나, 실제로 사용할 수 있는 권한은 획득하지 못한 경우
				// 데이터 접근을 그만두기 위해 내부 참조 횟수를 1씩 감소시킴
				// 이 때 참조 횟수를 0으로 바꾸는 마지막 스레드(1 - 1 = 0, 이전은 1)가 데이터 해제를 책임
				delete Ptr;
			}
		}
	}

private:
	struct FNode;

	struct FRefCountedObject
	{
		FNode* Ptr;
		int32 NumExternalRefs;
	};

	struct FNode
	{
		explicit FNode(const T& Value)
			: Data(make_shared<T>(Value))
		{
		}

		shared_ptr<T> Data;
		atomic<int32> NumInternalRefs;
		FRefCountedObject Next;
	};

private:
	void IncreaseNumRef(FRefCountedObject& Prev)
	{
		while (true)
		{
			FRefCountedObject NewObject = Prev;
			++NewObject.NumExternalRefs;

			// 참조 횟수를 1 증가시킨 스레드가 나 자신인지 확인하는 코드
			if (Head.compare_exchange_strong(Prev, NewObject))
			{
				Prev.NumExternalRefs = NewObject.NumExternalRefs;
				break;
			}
		}
	}

	atomic<FRefCountedObject> Head;	// 참조 횟수 정보가 있는 노드 포인터를 atomic하게 관리
};

