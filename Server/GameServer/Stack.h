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
			// ���� Ƚ���� 1 ������Ų ������� �ش� �����Ϳ� ������ �� �ִ� ������ �־���
			IncreaseNumRef(Prev);

			FNode* Ptr = Prev.Ptr;
			if (Ptr == nullptr)
			{
				return shared_ptr<T>();
			}

			// Ptr(������)->Next�� Head�� ������ ������� �ش� �����͸� ����� �� �ִ� ������ �־���
			if (Head.compare_exchange_strong(Prev, Ptr->Next))
			{
				// ������ ���� Ƚ�� 1 ������Ų �����Ͷ�� �̰��� ����
				// �ٸ� �����尡 ���� Ƚ���� �������״ٸ� Prev != Head(���� Ƚ���� �ٸ�)
				// ������ ��ȣǥ ����(��: 1 -> 2 / �ٸ� ������: 2 -> 3)

				shared_ptr<T> Resource;
				Resource.swap(Ptr->Data);

				/**
				 *	FRefCountedObject�� NumExternalRefs �⺻�� 1, �ڽ��� �����常 �ִٸ� ���� 2�� ���õ�����(������)
				 *	FNode�� NumRefs�� �⺻�� 0(������)
				 */

				// �ٸ� �����尡 �� �����Ϳ� �������̸� �����ϸ� �ȵ�! �̸� Ȯ��
				const int32 NumRefsIncreased = Prev.NumExternalRefs - 2;	// ���ۿ� �����ٸ� 2 - 0 = 0

				// �� �Ʒ� ������ {NumInternalRefs(0) + 0} �� 0(���� ���� ��) == -0 -> ������� �� �ۿ� ����
				// �� �ܿ� �ٸ� �����尡 �ִٸ� {0 + 2(����)} �� 0 == -2 -> �ٸ� �����尡 ���� ���̶� �����ϸ� �ȵ�!
				if (Ptr->NumInternalRefs.fetch_add(NumRefsIncreased) == -NumRefsIncreased)
				{
					delete Ptr;
				}

				return Resource;
			}
			else if (Ptr->NumInternalRefs.fetch_sub(1) == 1)
			{
				// ������ ���ٿ� ����������, ������ ����� �� �ִ� ������ ȹ������ ���� ���
				// ������ ������ �׸��α� ���� ���� ���� Ƚ���� 1�� ���ҽ�Ŵ
				// �� �� ���� Ƚ���� 0���� �ٲٴ� ������ ������(1 - 1 = 0, ������ 1)�� ������ ������ å��
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

			// ���� Ƚ���� 1 ������Ų �����尡 �� �ڽ����� Ȯ���ϴ� �ڵ�
			if (Head.compare_exchange_strong(Prev, NewObject))
			{
				Prev.NumExternalRefs = NewObject.NumExternalRefs;
				break;
			}
		}
	}

	atomic<FRefCountedObject> Head;	// ���� Ƚ�� ������ �ִ� ��� �����͸� atomic�ϰ� ����
};

