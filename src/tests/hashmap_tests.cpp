#include "hashmap/SeparateChainingHashMap.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

void test_insert_and_lookup() {
    SeparateChainingHashMap<int, int> map(8, 0.75f);

    assert(!map.get(42).has_value());

    map.insert(42, 100);
    assert(map.get(42).value() == 100);
}

void test_duplicate_updates_value() {
    SeparateChainingHashMap<int, int> map(8, 0.75f);

    map.insert(42, 100);
    map.insert(42, 200);

    assert(map.get(42).value() == 200);
}

void test_remove() {
    SeparateChainingHashMap<int, int> map(8, 0.75f);

    map.insert(1, 10);
    map.insert(2, 20);

    map.remove(1);
    assert(!map.get(1).has_value());
    assert(map.get(2).value() == 20);

    map.remove(999); // Removing a missing key should be harmless.
    assert(map.get(2).value() == 20);
}

void test_rehash_preserves_entries() {
    SeparateChainingHashMap<int, int> map(2, 0.75f);

    // Force multiple resizes.
    for (int key = 0; key < 100; ++key) {
        map.insert(key, key * 10);
    }

    for (int key = 0; key < 100; ++key) {
        assert(map.get(key).value() == key * 10);
    }
}

void test_zero_capacity_rejected() {
    bool threw = false;

    try {
        SeparateChainingHashMap<int, int> map(0, 0.75f);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);
}

int main() {
    test_insert_and_lookup();
    test_duplicate_updates_value();
    test_remove();
    test_rehash_preserves_entries();
    test_zero_capacity_rejected();

    std::cout << "All tests passed\n";
}