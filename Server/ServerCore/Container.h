#pragma once

#include "Types.h"
#include "Allocator.h"
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template<typename T>
using TArray = vector<T, TAllocator<T>>;

template<typename T>
using TList = list<T, TAllocator<T>>;

template<typename Key, typename Type, typename Pred = less<Key>>
using TSortedMap = map<Key, Type, Pred, TAllocator<pair<const Key, Type>>>;

template<typename Key, typename Pred = less<Key>>
using TSortedSet = set<Key, Pred, TAllocator<Key>>;

template<typename Key, typename Type, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using TMap = unordered_map<Key, Type, Hasher, KeyEq, TAllocator<pair<const Key, Type>>>;

template<typename Key, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using TSet = unordered_set<Key, Hasher, KeyEq, TAllocator<Key>>;

template<typename T>
using TDeque = deque<T, TAllocator<T>>;

template<typename T, typename Container = TDeque<T>>
using TQueue = queue<T, Container>;

template<typename T, typename Container = TArray<T>, typename Pred = less<typename Container::value_type>>
using TPriorityQueue = priority_queue<T, Container, Pred>;

using FString = basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t>>;