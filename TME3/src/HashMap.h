#pragma once
#include <iostream>
#include <vector>
#include <forward_list>

using namespace std;

template <typename K, typename V>
class HashMap {
public:

    struct Entry {
        const K key;
        V value;
    };

    struct notConstEntry {
        K key;
        V value;

        notConstEntry(const K& k, const V& v) : key(k), value(v) {}
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

    HashMap(size_t sz) : buckets(sz) {}

    V* get(const K& key) {
        size_t h = hash<K>()(key);
        size_t index = h % buckets.size();

        auto& list = buckets[index];
        for (auto& ent : list) {
            if (ent.key == key) {
                return &(ent.value);
            }
        }
        return nullptr;
    }

    bool put(const K& key, const V& value) {
        size_t h = hash<K>()(key);
        size_t index = h % buckets.size();

        auto& list = buckets[index];
        for (auto& ent : list) {
            if (ent.key == key) {
                ent.value = value;
                return true;
            }
        }
        list.push_front(Entry{ key, value });
        return false;
    }

    size_t size() {
        size_t currentSize = 0;
        for (size_t i = 0; i < buckets.size(); ++i) {
            const auto& list = buckets[i];
            for (const auto& ent : list) {
                currentSize++;
            }
        }
        return currentSize;
    }

    class iterator {
        buckets_t& buckets;
        size_t vit;
        typename forward_list<Entry>::iterator lit;

    public:
        iterator(buckets_t& bucketsV, size_t vitV = 0)
            : buckets(bucketsV), vit(vitV) {
            while (vit < buckets.size() && buckets[vit].empty()) {
                ++vit;
            }
            if (vit < buckets.size()) {
                lit = buckets[vit].begin();
            }
        }

        iterator(buckets_t& bucketsV, size_t vitV, typename forward_list<Entry>::iterator litV)
            : buckets(bucketsV), vit(vitV), lit(litV) {}

        iterator& operator++() {
            ++lit;
            while (vit < buckets.size() && lit == buckets[vit].end()) {
                ++vit;
                if (vit < buckets.size()) {
                    lit = buckets[vit].begin();
                }
            }
            return *this;
        }

        bool operator!=(const iterator& other) const {
            if (vit != other.vit)
                return true;
            if (vit >= buckets.size())
                return false;
            return lit != other.lit;
        }

        notConstEntry operator*() const {
            return notConstEntry(*lit);
        }
    };

    iterator begin() {
        return iterator(buckets);
    }

    iterator end() {
        return iterator(buckets, buckets.size());
    }
};
