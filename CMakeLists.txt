cmake_minimum_required(VERSION 3.5)
project(tiny-vdbms)
# add_executable(app src/test/meta/vector_test.cpp)
# add_executable(app src/test/distance/basic_distance.cpp)
# add_executable(app src/test/distance/euclidean_distance_test.cpp)
# add_executable(app src/test/distance/inner_product_test.cpp)

# include_directories(${PROJECT_SOURCE_DIR}/include)
# file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


# add_subdirectory(./src)
# add_subdirectory(./include)

add_executable(app src/test/bench/demo_bench.cpp src/memory/memory_management.cpp)