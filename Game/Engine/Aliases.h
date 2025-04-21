#pragma once

// SmartPointerAliases.h
#pragma once

#include <memory>
#include <utility>  // for std::pair
#include <type_traits>

//
// Unreal-Style Smart Pointers
//

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

template <typename T>
using TUniquePtr = std::unique_ptr<T>;

template <typename T>
using TSharedFromThis = std::enable_shared_from_this<T>;

template <typename T>
TSharedPtr<T> AsShared(T* ThisPtr)
{
    static_assert(std::is_base_of_v<TSharedFromThis<T>, T>, "T must inherit from enable_shared_from_this");
    return ThisPtr->shared_from_this();
}

//
// Unreal-Style Make Helpers
//

template <typename T, typename... Args>
TSharedPtr<T> MakeShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
TUniquePtr<T> MakeUnique(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

//
// Null & Validity Checker (IsValid)
//

template <typename T>
bool IsValid(const TWeakPtr<T>& Weak)
{
    return !Weak.expired();
}

template <typename T>
bool IsValid(const TSharedPtr<T>& Ptr)
{
    return Ptr != nullptr;
}

//
// Unreal-Style Casting Helpers
//

template <typename To, typename From>
__forceinline constexpr To ConstexprCast(const From& Value)
{
    return static_cast<To>(Value);
}

// Static cast
template <typename To, typename From>
__forceinline To* StaticCast(From* Ptr)
{
    return static_cast<To*>(Ptr);
}

template <typename To, typename From>
__forceinline To StaticCast(const From& Value)
{
    return static_cast<To>(Value);
}

// Dynamic cast
template <typename To, typename From>
__forceinline To* DynamicCast(From* Ptr)
{
    return dynamic_cast<To*>(Ptr);
}

// Const cast
template <typename To, typename From>
__forceinline To* ConstCast(From* Ptr)
{
    return const_cast<To*>(Ptr);
}

template <typename To, typename From>
__forceinline To ConstCast(const From& value)
{
    return const_cast<To>(value);
}

// Reinterpret cast
template <typename To, typename From>
__forceinline To* ReinterpretCast(From* Ptr)
{
    return reinterpret_cast<To*>(Ptr);
}

template <typename To, typename From>
__forceinline To ReinterpretCast(const From& value)
{
    return reinterpret_cast<To>(value);
}

//
// Unreal-Style Pointer Cast Helpers
//

template <typename To, typename From>
TSharedPtr<To> StaticCastSharedPtr(const TSharedPtr<From>& Ptr)
{
    return std::static_pointer_cast<To>(Ptr);
}

template <typename To, typename From>
TSharedPtr<To> DynamicCastSharedPtr(const TSharedPtr<From>& Ptr)
{
    return std::dynamic_pointer_cast<To>(Ptr);
}

