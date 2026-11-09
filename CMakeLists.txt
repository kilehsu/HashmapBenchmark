cmake_minimum_required(VERSION 3.20)

project(HashmapBenchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Build the library without its own test suite.
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.4
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(googlebenchmark)

add_executable(main src/main.cpp) 

target_include_directories(main PRIVATE include)

target_link_libraries(
    main
    PRIVATE benchmark::benchmark_main
)

enable_testing()

add_executable(hashmap_tests src/tests/separate_chaining_hash_map_tests.cpp)
target_include_directories(hashmap_tests PRIVATE include)

add_test(NAME hashmap_tests COMMAND hashmap_tests)

add_executable(hashmap_benchmark src/benchmark/benchmark.cpp)
target_include_directories(hashmap_benchmark PRIVATE include)
target_link_libraries(
    hashmap_benchmark
    PRIVATE benchmark::benchmark_main
)
