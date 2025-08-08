#pragma once
#include "Types.h"

#pragma region TypeList
template<typename... Types>
struct TypeList;

template<typename Type, typename OtherType>
struct TypeList<Type, OtherType>
{
	using First = Type;
	using Second = OtherType;
};

template<typename Type, typename... OtherTypes>
struct TypeList<Type, OtherTypes...>
{
	using First = Type;
	using Second = TypeList<OtherTypes...>;
};

#pragma endregion

#pragma region SizeOf
template<typename Type>
struct SizeOf;

template<>
struct SizeOf<TypeList<>>
{
	static constexpr size_t Value = 0;
};

template<typename Type, typename... OtherTypes>
struct SizeOf<TypeList<Type, OtherTypes...>>
{
	static constexpr size_t Value = 1 + SizeOf<TypeList<OtherTypes...>>::Value;
};

#pragma endregion

#pragma region TypeAt
template<typename TTuple, int32 Index>
struct TypeAt;

template<typename Type, typename... OtherTypes>
struct TypeAt<TypeList<Type, OtherTypes...>, 0>
{
	using Value = Type;
};

template<typename Type, typename... OtherTypes, int32 Index>
struct TypeAt<TypeList<Type, OtherTypes...>, Index>
{
	using Value = typename TypeAt<TypeList<OtherTypes...>, Index - 1>::Value;
};

#pragma endregion

#pragma region IndexOf
template<typename TTuple, typename Type>
struct IndexOf;

template<typename... Types, typename TargetType>
struct IndexOf<TypeList<TargetType, Types...>, TargetType>
{
	static constexpr size_t Value = 0;
};

template<typename Type>
struct IndexOf<TypeList<>, Type>
{
	static constexpr size_t Value = -1;
};

template<typename Type, typename... OtherTypes, typename TargetType>
struct IndexOf<TypeList<Type, OtherTypes...>, TargetType>
{
private:
	static constexpr size_t Temp = IndexOf<TypeList<OtherTypes...>, TargetType>::Value;

public:
	static constexpr size_t Value = (Temp == -1) ? -1 : Temp + 1;
};

#pragma endregion

#pragma region Convert
template<typename From, typename To>
class Convert
{
private:
	// 아무 값이나 상관없음
	using Small = int8;
	using Big = int32;

	// 의미 없는 반환값
	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }

public:
	// MakeFrom으로 From을 Test에 넘겼을 때, Small이 반횐되냐 Big이 반환되냐에 따라 결과 결정
	// Small이 반환되면 To로 캐스팅 가능한 것, Big이 반환되면 캐스팅 불가능한 것
	static constexpr size_t Result = sizeof(Test(MakeFrom())) == sizeof(Small);
};
#pragma endregion

#pragma region Multi Conversion

// 정수값을 일종의 템플릿 클래스처럼 취급하도록
template<int32 Value>
struct Integer
{
	static constexpr int32 Type = Value;
};

template<typename TTuple>
class MultiConversion
{
public:
	static constexpr size_t Size = SizeOf<TTuple>::Value;
	inline static bool Table[Size][Size];

	MultiConversion()
	{
		Set(Integer<0>(), Integer<0>());
	}

	template<int32 FromIndex, int32 ToIndex>
	static void Set(Integer<FromIndex>, Integer<ToIndex>)
	{
		using From = typename TypeAt<TTuple, FromIndex>::Value;
		using To = typename TypeAt<TTuple, ToIndex>::Value;

		Table[FromIndex][ToIndex] = Convert<const From*, const To*>::Result;
		Set(Integer<FromIndex>(), Integer<ToIndex + 1>());
	}

	template<int32 Row>	// Col Ended
	static void Set(Integer<Row>, Integer<Size>)
	{
		Set(Integer<Row + 1>(), Integer<0>());
	}

	template<int32 Col>	// Row & Col Ended
	static void Set(Integer<Size>, Integer<Col>)
	{
		// Set Finished
	}

	static bool CanCast(int32 FromIndex, int32 ToIndex)
	{
		static MultiConversion Conversions;
		return Table[FromIndex][ToIndex];
	}
};

#pragma endregion

// template<typename To, typename From>
// To Cast(From* InPtr)
// {
// 	if (InPtr == nullptr)
// 	{
// 		return nullptr;
// 	}
//
// 	using Types = typename From::Types;
// 	if (MultiConversion<Types>::CanCast(
// 		InPtr->TypeId,
// 		IndexOf<Types, remove_pointer_t<To>>::Value)
// 		)
// 	{
// 		return static_cast<To>(InPtr);
// 	}
//
// 	return nullptr;
// }
//
// template<typename To, typename From>
// shared_ptr<To> Cast(shared_ptr<From> InPtr)
// {
// 	if (InPtr == nullptr)
// 	{
// 		return nullptr;
// 	}
//
// 	using Types = typename From::Types;
// 	if (MultiConversion<Types>::CanCast(
// 		InPtr->TypeId,
// 		IndexOf<Types, remove_pointer_t<To>>::Value)
// 		)
// 	{
// 		return static_pointer_cast<To>(InPtr);
// 	}
//
// 	return nullptr;
// }
//
// template<typename To, typename From>
// bool IsA(From* InPtr)
// {
// 	if (InPtr == nullptr)
// 	{
// 		return false;
// 	}
//
// 	using Types = typename From::Types;
// 	return MultiConversion<Types>::CanCast(
// 		InPtr->TypeId,
// 		IndexOf<Types, remove_pointer_t<To>>::Value
// 	);
// }
//
// template<typename To, typename From>
// bool IsA(shared_ptr<From> InPtr)
// {
// 	return IsA<To, From>(InPtr);
// }

template<typename To, typename From>
shared_ptr<To> StaticCastSharedPtr(const shared_ptr<From>& InPtr)
{
	return static_pointer_cast<To>(InPtr);
}

template<typename To, typename From>
shared_ptr<To> ConstCastSharedPtr(const shared_ptr<From>& InPtr)
{
	return const_pointer_cast<To>(InPtr);
}
