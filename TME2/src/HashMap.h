/*
 * HashMap.h
 *
 *  Created on: 25 sept. 2024
 *      Author: taryck
 */

#pragma once
#include <iostream>
#include <vector>
#include <forward_list>

using namespace std;
template <typename K, typename V>
class HashMap {
public :

	struct Entry {
		const K key;
		V value;
	};

	struct notConstEntry {
		K key;
		V value;
		notConstEntry(const std::string& k, int v) : key(k), value(v) {}
		notConstEntry(const Entry& autre) : key(autre.key), value(autre.value) {}
		notConstEntry& operator=(const Entry& autre) {
				        if (this != &autre) {
				            key = autre.key;
				            value = autre.value;
				        }
				        return *this;
				}
		notConstEntry& operator=(const notConstEntry& autre) {
						        if (this != &autre) {
						            key = autre.key;
						            value = autre.value;
						        }
						        return *this;
						}
	};
	typedef vector<forward_list<Entry>> buckets_t;
	buckets_t buckets;

	HashMap(size_t sz): buckets(sz) {}

	V* get(const K& key) {
		size_t h=hash<K>()(key);
		size_t index=h%buckets.size();

		auto& list = buckets[index];
		for (auto& ent:list) {
			if (ent.key == key) {
				return &(ent.value);
			}
		}
		return nullptr;
	}

	bool put (const K& key, const V& value) {
		size_t h=hash<K>()(key);
		size_t index=h%buckets.size();

		auto& list = buckets[index];
		for (auto& ent:list) {
			if (ent.key == key) {
				ent.value = value;
				return true;
			}
		}
		list.push_front(Entry{key, value});
		return false;
	}

	size_t size() {
		size_t currentSize = 0;
		for (size_t i=0; i< buckets.size(); ++i) {
				const auto& list = buckets[i];
				for (const auto& ent:list) {
						currentSize++;
				}
		}
		return currentSize;
	}

};


