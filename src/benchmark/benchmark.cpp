#include <algorithm>
#include <benchmark/benchmark.h>
#include <hashmap/separate_chaining_hash_map.hpp>
#include <random>
#include <utility>
#include <vector>

static void BM_insert(benchmark::State& state) {
    const int capacity = static_cast<int>(state.range(0));
    const float max_load_factor = static_cast<float>(state.range(1)) / 100.0f;
    std::mt19937_64 gen(42);
    std::uniform_int_distribution<int> dis(1, 10000);

    std::vector<std::pair<int, int>> inputs;
    inputs.reserve(1024);

    for (int i = 0; i < 1024; ++i) {
        const int key = dis(gen);
        const int value = dis(gen);
        inputs.emplace_back(key, value);
    }
    for (auto _ : state) {
        state.PauseTiming();
        {
            SeparateChainingHashMap<int, int> hashmap(capacity,
                                                      max_load_factor);
            auto* map_ptr = &hashmap;
            benchmark::DoNotOptimize(map_ptr);

            state.ResumeTiming();

            for (const auto& [key, value] : inputs) {
                hashmap.insert(key, value);
            }

            benchmark::ClobberMemory();
            state.PauseTiming();
        }
        state.ResumeTiming();
    }
    state.SetItemsProcessed(state.iterations() * 1024);
}

static void BM_get(benchmark::State& state) {
    std::mt19937_64 gen(42);
    std::uniform_int_distribution<int> dis(1, 10000);

    std::vector<std::pair<int, int>> inputs;
    inputs.reserve(1024);
    SeparateChainingHashMap<int, int> hashmap(1024, 1.0f);
    auto* map_ptr = &hashmap;

    for (int i = 0; i < 1024; ++i) {
        const int key = dis(gen);
        const int value = dis(gen);
        inputs.emplace_back(key, value);
        hashmap.insert(key, value);
    }

    std::shuffle(inputs.begin(), inputs.end(), gen);
    for (auto _ : state) {
        benchmark::DoNotOptimize(map_ptr);
        for (const auto& [key, _] : inputs) {
            auto res = hashmap.get(key);
            benchmark::DoNotOptimize(res);
        }
    }
    state.SetItemsProcessed(state.iterations() * 1024);
}

// BENCHMARK(BM_insert)
//     ->ArgNames({"capacity", "max_load_percent"})
//     ->Args({8, 25})
//     ->Args({8, 50})
//     ->Args({8, 75})
//     ->Args({8, 100})
//     ->Args({8, 200})
//     ->Args({2048, 100});

BENCHMARK(BM_get);

BENCHMARK_MAIN();