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

add_executable(hashmap_bench src/main.cpp) 

target_include_directories(hashmap_bench PRIVATE include)

target_link_libraries(
    hashmap_bench
    PRIVATE benchmark::benchmark_main
)

enable_testing()

add_executable(hashmap_tests src/tests/hashmap_tests.cpp)
target_include_directories(hashmap_tests PRIVATE include)

add_test(NAME hashmap_tests COMMAND hashmap_tests)
