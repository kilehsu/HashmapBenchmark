#pragma once

#include "open_addressing_hashmap.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename K, typename V>
OpenAddressingHashMap<K, V>::Slot::Slot(State state,
                                        std::optional<std::pair<K, V>> entry)
    : state_(state), entry_(entry) {}

template <typename K, typename V>
OpenAddressingHashMap<K, V>::ProbeResult::ProbeResult(
    bool found, std::optional<size_t> index)
    : found_(found), index_(index) {}

template <typename K, typename V>
OpenAddressingHashMap<K, V>::OpenAddressingHashMap(std::size_t capacity,
                                                   float max_load_factor)
    : size_(0), max_loading_factor_(max_load_factor) {
    if (capacity <= 0) {
        throw std::invalid_argument("Capacity must be positive");
    }

    if (!std::isfinite(max_load_factor) || max_load_factor <= 0.0f ||
        max_load_factor > 1.0f) {
        throw std::invalid_argument(
            "Maximum load factor must be finite and positive");
    }

    table_.resize(capacity);
}

template <typename K, typename V>
std::size_t
OpenAddressingHashMap<K, V>::hash(const K& key,
                                  const std::vector<Slot>& table) const {
    return std::hash<K>{}(key) % table.size();
}

template <typename K, typename V>
typename OpenAddressingHashMap<K, V>::ProbeResult
OpenAddressingHashMap<K, V>::probe(const K& key,
                                   const std::vector<Slot>& table) const {
    std::size_t idx = hash(key, table);
    std::size_t last_deleted = table.size();

    for (std::size_t checked = 0; checked < table.size(); ++checked) {
        const Slot& slot = table[idx];

        if (slot.state_ == State::DELETED && last_deleted == table.size()) {
            last_deleted = idx;
        }

        if (slot.state_ == State::EMPTY && last_deleted == table.size()) {
            return ProbeResult(false, idx);
        }
        if (slot.state_ == State::EMPTY && last_deleted < table.size()) {
            return ProbeResult(false, last_deleted);
        }
        if (slot.state_ == State::OCCUPIED && slot.entry_->first == key) {
            return ProbeResult(true, idx);
        }

        idx = (idx + 1) % table.size();
    }

    if (last_deleted < table.size()) {
        return ProbeResult(false, last_deleted);
    }

    return ProbeResult(false, std::nullopt);
}

template <typename K, typename V>
typename OpenAddressingHashMap<K, V>::ProbeResult
OpenAddressingHashMap<K, V>::probe_get(const K& key,
                                       const std::vector<Slot>& table) const {
    std::size_t idx = hash(key, table);

    for (std::size_t checked = 0; checked < table.size(); ++checked) {
        const Slot& slot = table[idx];

        if (slot.state_ == State::EMPTY) {
            return ProbeResult(false, std::nullopt);
        }

        if (slot.state_ == State::OCCUPIED && slot.entry_->first == key) {
            return ProbeResult(true, idx);
        }

        idx = (idx + 1) % table.size();
    }

    return ProbeResult(false, std::nullopt);
}

template <typename K, typename V>
typename std::optional<V> OpenAddressingHashMap<K, V>::get(const K& key) const {
    ProbeResult res = probe(key, table_);
    if (!res.found_) {
        return std::nullopt;
    }

    return table_[res.index_.value()].entry_->second;
}

template <typename K, typename V>
float OpenAddressingHashMap<K, V>::get_load_factor(std::size_t size) const {
    return static_cast<float>(size) / table_.size();
}

template <typename K, typename V>
void OpenAddressingHashMap<K, V>::insert(const K& key, const V& value) {
    ProbeResult res = probe(key, table_);
    if (res.found_) {
        table_[res.index_.value()].entry_->second = value;
        return;
    }

    const std::size_t limit = table_.max_size();
    if (size_ >= limit) {
        throw std::length_error("Hash map capacity limit exceeded");
    }
    const std::size_t projected_size = size_ + 1;

    if (static_cast<long double>(projected_size) / table_.size() >
        max_loading_factor_) {
        const long double required = std::ceil(
            static_cast<long double>(projected_size) / max_loading_factor_);
        // The exclusive integer bound is an exactly representable power of two.
        const long double size_t_bound =
            std::ldexp(1.0L, std::numeric_limits<std::size_t>::digits);
        if (!std::isfinite(required) || required >= size_t_bound) {
            throw std::length_error("Hash map capacity limit exceeded");
        }
        const std::size_t minimum_capacity = static_cast<std::size_t>(required);
        if (minimum_capacity > limit) {
            throw std::length_error("Hash map capacity limit exceeded");
        }

        const std::size_t new_capacity =
            table_.size() <= limit / 2
                ? std::max(table_.size() * 2, minimum_capacity)
                : minimum_capacity;

        rehash_table(new_capacity);

        res = probe(key, table_);
    }

    Slot& slot = table_[res.index_.value()];
    slot.entry_.emplace(key, value);

    slot.state_ = State::OCCUPIED;
    size_ += 1;

    return;
}

template <typename K, typename V>
void OpenAddressingHashMap<K, V>::rehash_table(std::size_t new_capacity) {
    std::vector<Slot> new_table(new_capacity);

    for (const Slot& slot : table_) {
        if (slot.state_ == State::OCCUPIED) {
            ProbeResult res = probe(slot.entry_->first, new_table);
            Slot& new_slot = new_table[res.index_.value()];
            new_slot.entry_.emplace(slot.entry_->first, slot.entry_->second);
            new_slot.state_ = State::OCCUPIED;
        }
    }

    table_ = std::move(new_table);
}

template <typename K, typename V>
void OpenAddressingHashMap<K, V>::remove(const K& key) {
    ProbeResult res = probe(key, table_);

    if (!res.found_) {
        return;
    }

    Slot& slot = table_[res.index_.value()];

    slot.entry_.reset();
    slot.state_ = State::DELETED;

    size_ -= 1;

    return;
}