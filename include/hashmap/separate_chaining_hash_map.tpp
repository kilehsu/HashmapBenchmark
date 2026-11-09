#pragma once
#include "separate_chaining_hash_map.hpp"
#include <cmath>
#include <cstddef>
#include <functional>
#include <list>
#include <optional>
#include <stdexcept>
#include <vector>

template <typename K, typename V>
SeparateChainingHashMap<K, V>::Entry::Entry(K key, V value)
    : key_(key), value_(value) {}

template <typename K, typename V>
SeparateChainingHashMap<K, V>::SeparateChainingHashMap(std::size_t capacity,
                                                       float max_load_factor)
    : size_(0), max_loading_factor_(max_load_factor) {
    if (capacity <= 0) {
        throw std::invalid_argument("Capacity must be positive");
    }

    if (!std::isfinite(max_load_factor) || max_load_factor <= 0.0f) {
        throw std::invalid_argument(
            "Maximum load factor must be finite and positive");
    }

    table_.resize(capacity);
}

template <typename K, typename V>
void SeparateChainingHashMap<K, V>::insert(const K& key, const V& value) {
    std::size_t bucket_idx = hash(key);
    auto& lst = table_.at(bucket_idx);

    for (Entry& entry : lst) {
        if (entry.key_ == key) {
            entry.value_ = value;
            return;
        }
    }

    lst.emplace_back(key, value);
    ++size_;

    while (get_load_factor() > max_loading_factor_) {
        rehash_buckets();
    }
}

template <typename K, typename V>
std::optional<V> SeparateChainingHashMap<K, V>::get(const K& key) const {
    std::size_t bucket_idx = hash(key);
    const auto& lst = table_.at(bucket_idx);

    for (const Entry& entry : lst) {
        if (entry.key_ == key) {
            return entry.value_;
        }
    }

    return std::nullopt;
}

template <typename K, typename V>
void SeparateChainingHashMap<K, V>::remove(const K& key) {
    std::size_t bucket_idx = hash(key);
    auto& lst = table_.at(bucket_idx);
    const auto removed = std::erase_if(
        lst, [&](const Entry& entry) { return entry.key_ == key; });

    size_ -= static_cast<int>(removed);
}

template <typename K, typename V>
std::size_t SeparateChainingHashMap<K, V>::hash(const K& key) const {
    return std::hash<K>{}(key) % table_.size();
}

template <typename K, typename V>
float SeparateChainingHashMap<K, V>::get_load_factor() const {
    return static_cast<float>(size_) / table_.size();
}

template <typename K, typename V>
void SeparateChainingHashMap<K, V>::rehash_buckets() {
    std::size_t new_capacity = table_.size() * 2;
    std::vector<std::list<Entry>> new_table(new_capacity);

    for (auto& lst : table_) {
        for (Entry& entry : lst) {
            std::size_t bucket_idx = std::hash<K>{}(entry.key_) % new_capacity;
            auto& new_lst = new_table.at(bucket_idx);
            new_lst.emplace_back(entry.key_, entry.value_);
        }
    }

    table_ = std::move(new_table);
}