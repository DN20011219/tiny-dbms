cmake_minimum_required(VERSION 3.5)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)

project(tiny-vdbms)

set(CMAKE_C_COMPILER clang)

add_executable(
    app 
src/test/bench/demo_bench.cpp 
include/storage/memory/block_slot.cpp
include/storage/memory/easy_replacer.cpp
include/storage/memory/buffer_pool.cpp
include/storage/memory/lock_watcher.cpp
)

# Add a custom command to execute pre_set.sh before building the app
add_custom_command(
    TARGET app
    PRE_BUILD
    COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/pre_set.sh
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/pre_set.sh
)