#include "hashmap/open_addressing_hashmap.hpp"

#include <cassert>

void test_rehash_preserves_entries() {
    OpenAddressingHashMap<int, int> map(2, 0.5f);

    // Force multiple resizes.
    for (int key = 0; key < 100; ++key) {
        map.insert(key, key * 10);
    }

    for (int key = 0; key < 100; ++key) {
        assert(map.get(key).value() == key * 10);
    }
}

void test_remove() {
    OpenAddressingHashMap<int, int> map(8, 0.5f);
    map.insert(1, 2);
    map.insert(9, 18);

    assert(map.get(1).value() == 2);

    map.remove(1);

    assert(!map.get(1).has_value());

    assert(map.get(9).value() == 18);

    map.remove(999);

    assert(map.get(9).value() == 18);
}

int main() {
    test_rehash_preserves_entries();
    test_remove();
}
