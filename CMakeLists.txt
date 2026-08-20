cmake_minimum_required(VERSION 3.20)

project(master_rad_esa LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(master_rad_esa
    src/main.cpp
    src/suffix_array.cpp
    src/lcp.cpp
    src/esa.cpp
    src/repeats.cpp
    src/fasta.cpp
    src/output.cpp
    src/memory.cpp
    src/benchmark.cpp
)

target_include_directories(master_rad_esa
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

enable_testing()

add_executable(
    test_suffix_array
    tests/test_suffix_array.cpp
    src/suffix_array.cpp
)

target_include_directories(
    test_suffix_array
    PRIVATE
    include
)

target_compile_features(
    test_suffix_array
    PRIVATE
    cxx_std_17
)

add_test(
    NAME suffix_array_tests
    COMMAND test_suffix_array
)


add_executable(
    test_repeats
    tests/test_repeats.cpp
    src/suffix_array.cpp
    src/lcp.cpp
    src/esa.cpp
    src/repeats.cpp
)

target_include_directories(
    test_repeats
    PRIVATE
    include
)

target_compile_features(
    test_repeats
    PRIVATE
    cxx_std_17
)

add_test(
    NAME repeat_tests
    COMMAND test_repeats
)

add_executable(
    test_fasta
    tests/test_fasta.cpp
    src/fasta.cpp
)

target_include_directories(
    test_fasta
    PRIVATE
    include
)

target_compile_features(
    test_fasta
    PRIVATE
    cxx_std_17
)

add_test(
    NAME fasta_tests
    COMMAND test_fasta
)

set_tests_properties(
    fasta_tests
    PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)