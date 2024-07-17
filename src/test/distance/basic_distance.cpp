// Copyright (c) 2024 by dingning
//
// file  : vector.h
// since : 2024-07-16
// desc  : basic vector struct, support different type of data and length (such as int, size_t, long, unsigned int)

#include <iostream>
#include "../../../include/config.h"
#include "../../../include/distance/basic_distance.h"

#define TestDistance BasicDistance<double>

using namespace tiny_v_dbms;

int main() {
    double data[5] = {5, 4, 3, 2, 1};
    BASE_VECTOR first_vector(data, 5);

    double second_data[5] = {1, 2, 1, 4, 5};
    BASE_VECTOR second_vector(second_data, 5);

    std::cout << "test begin" << std::endl;

    TestDistance test_object;
    double result = test_object.Cal(first_vector, second_vector);

    std::cout << "result" << result << std::endl;

    std::cout << "test end" << std::endl;
}