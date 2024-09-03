cmake_minimum_required(VERSION 3.5)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)

project(tiny-vdbms)
# add_executable(app src/test/meta/vector_test.cpp)
# add_executable(app src/test/distance/basic_distance.cpp)
# add_executable(app src/test/distance/euclidean_distance_test.cpp)
# add_executable(app src/test/distance/inner_product_test.cpp)

# include_directories(${PROJECT_SOURCE_DIR}/include)
# file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


# add_subdirectory(./src)
# add_subdirectory(./include)

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