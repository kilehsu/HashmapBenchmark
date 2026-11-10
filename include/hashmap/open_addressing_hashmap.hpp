#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

template <typename K, typename V>
class OpenAddressingHashMap {
private:
    enum struct State { OCCUPIED, DELETED, EMPTY };

    struct Slot {
        State state_ = State::EMPTY;
        std::optional<std::pair<K, V>> entry_;

        Slot() = default;
        Slot(State, std::optional<std::pair<K, V>>);
    };

    struct ProbeResult {
        bool found_;
        std::optional<std::size_t> index_;

        ProbeResult(bool, std::optional<std::size_t>);
    };

    ProbeResult probe(const K&) const;

    std::size_t hash(const K&) const;

    std::vector<Slot> table_;
    std::size_t size_;
    float max_loading_factor_;

public:
    OpenAddressingHashMap(std::size_t, float);
};

#include "open_addressing_hashmap.tpp"