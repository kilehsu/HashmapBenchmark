#pragma once

#include <cstddef>
#include <list>
#include <optional>
#include <vector>

template <typename K, typename V>
class SeparateChainingHashMap {
private:
    struct Entry {
        K key_;
        V value_;

        Entry(K key, V value);
    };

    std::vector<std::list<Entry>> table_;
    std::size_t size_;
    float max_loading_factor_;

public:
    SeparateChainingHashMap(std::size_t capacity, float max_load_factor);

    void insert(const K& key, const V& value);

    std::optional<V> get(const K& key) const;

    void remove(const K& key);

private:
    void rehash_buckets();
    float get_load_factor() const;
    std::size_t hash(const K& key) const;
};

#include "SeparateChainingHashMap.tpp"