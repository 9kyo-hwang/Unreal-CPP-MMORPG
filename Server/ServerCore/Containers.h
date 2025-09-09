#pragma once

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

template<typename T>
using TArray = std::vector<T>;

template<typename T>
using TList = std::list<T>;

template<typename T>
using TQueue = std::queue<T>;

template<typename TKey, typename TValue>
using TMap = std::unordered_map<TKey, TValue>;

template<typename T>
using TSet = std::unordered_set<T>;

template<typename T>
using TAtomic = std::atomic<T>;

template<typename T>
using TSharedPtr = std::shared_ptr<T>;

template<typename T>
using TUniquePtr = std::unique_ptr<T>;

template<typename T>
using TWeakPtr = std::weak_ptr<T>;

template<typename T>
using TFunction = std::function<T>;

template<typename T, typename... Args>
TSharedPtr<T> MakeShared(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
TUniquePtr<T> MakeUnique(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename ObjectType>
class TSharedFromThis : public std::enable_shared_from_this<ObjectType>
{
public:
	TSharedPtr<ObjectType> AsShared()
	{
		return this->shared_from_this();
	}

	TSharedPtr<ObjectType const> AsShared() const
	{
		return this->shared_from_this();
	}

	TWeakPtr<ObjectType> AsWeak()
	{
		return this->weak_from_this();
	}

	TWeakPtr<ObjectType const> AsWeak() const
	{
		return this->weak_from_this();
	}

	template<class OtherType>
	TSharedPtr<OtherType> SharedThis(OtherType* ThisPtr)
	{
		return std::static_pointer_cast<OtherType>(ThisPtr->shared_from_this());
	}

	template<class OtherType>
	TSharedPtr<OtherType const> SharedThis(const OtherType* ThisPtr)
	{
		return std::static_pointer_cast<OtherType>(ThisPtr->shared_from_this());
	}
};

