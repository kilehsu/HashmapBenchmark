#pragma once

#include "open_addressing_hashmap.hpp"

#include <cmath>
#include <cstddef>
#include <functional>
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
std::size_t OpenAddressingHashMap<K, V>::hash(const K& key) const {
    return std::hash<K>{}(key) % table_.size();
}

template <typename K, typename V>
typename OpenAddressingHashMap<K, V>::ProbeResult
OpenAddressingHashMap<K, V>::probe(const K& key) const {
    std::size_t idx = hash(key);
    std::size_t last_deleted = table_.size();

    for (std::size_t checked = 0; checked < table_.size(); ++checked) {
        const Slot& slot = table_[idx];

        if (slot.state_ == State::DELETED && last_deleted == table_.size()) {
            last_deleted = idx;
        }

        if (slot.state_ == State::EMPTY && last_deleted == table_.size()) {
            return ProbeResult(false, idx);
        }
        if (slot.state_ == State::EMPTY && last_deleted < table_.size()) {
            return ProbeResult(false, last_deleted);
        }
        if (slot.state_ == State::OCCUPIED && slot.entry_->first == key) {
            return ProbeResult(true, idx);
        }

        idx = (idx + 1) % table_.size();
    }

    if (last_deleted < table_.size()) {
        return ProbeResult(false, last_deleted);
    }

    return ProbeResult(false, std::nullopt);
}