#pragma once

#include "Types.h"
#include "Allocator.h"
#include <vector>
#include <array>
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template<typename ElementType>
using TArray = vector<ElementType, TAllocator<ElementType>>;

template<typename InElementType, uint32 NumElements>
using TStaticArray = array<InElementType, NumElements>;

template<typename ElementType>
using TList = list<ElementType, TAllocator<ElementType>>;

template<typename InKeyType, typename InValueType, typename SortPredicate = less<InKeyType>>
using TSortedMap = map<InKeyType, InValueType, SortPredicate, TAllocator<pair<const InKeyType, InValueType>>>;

template<typename InElementType, typename SortPredicate = less<InElementType>>
using TSortedSet = set<InElementType, SortPredicate, TAllocator<InElementType>>;

template<typename InKeyType, typename InValueType, typename Hasher = hash<InKeyType>, typename KeyFuncs = equal_to<InKeyType>>
using TMap = unordered_map<InKeyType, InValueType, Hasher, KeyFuncs, TAllocator<pair<const InKeyType, InValueType>>>;

template<typename InElementType, typename Hasher = hash<InElementType>, typename KeyFuncs = equal_to<InElementType>>
using TSet = unordered_set<InElementType, Hasher, KeyFuncs, TAllocator<InElementType>>;

template<typename InElementType>
using TDeque = deque<InElementType, TAllocator<InElementType>>;

template<typename InElementType, typename Container = TDeque<InElementType>>
using TQueue = queue<InElementType, Container>;

template<typename InElementType, typename Container = TDeque<InElementType>>
using TStack = stack<InElementType, Container>;

template<typename InElementType, typename Container = TArray<InElementType>, typename SortPredicate = less<typename Container::value_type>>
using TPriorityQueue = priority_queue<InElementType, Container, SortPredicate>;

using FString = basic_string<wchar_t, char_traits<wchar_t>, TAllocator<wchar_t>>;