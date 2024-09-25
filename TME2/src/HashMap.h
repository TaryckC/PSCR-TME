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
	struct Entry {
		const K key;
		V value;
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

		const auto& list = buckets[index];
		for (const auto& ent:list) {
			if (ent.key == key) {
				ent.value = value;
				return true;
			}
		}
		list.push_front(new Entry(key, value));
		return false;
	}

	size_t size() {
		size_t currentSize = 0;
		for (size_t i=0; i< buckets.size(); ++i) {
			if (buckets[i].size != 0) {
				const auto& list = buckets[i];
				for (const auto& ent:list) {
						currentSize++;
				}
			}
		}
		return currentSize;
	}

};


