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
)

target_include_directories(master_rad_esa
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)