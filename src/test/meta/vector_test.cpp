// Copyright (c) 2024 by dingning
//
// file  : vector.h
// since : 2024-07-16
// desc  : basic vector struct, support different type of data and length (such as int, size_t, long, unsigned int)

#include <iostream>
#include "../../../include/meta/vector.h"

#define TestVector Vector<int, int>

using namespace tiny_v_dbms;

int main() {

    TestVector vector(10);

    int data[5] = {1, 2, 3, 4, 5};
    TestVector data_vector(data, 5);

    std::cout << "test begin" << std::endl;

    int first = data_vector.Get(0);
    std::cout << "1 " << first << std::endl;
    int mid = data_vector.Get(2);
    std::cout << "test begin " << mid << std::endl;
    int last = data_vector.Get(4);
    std::cout << "test begin "<< last << std::endl;

    // int out_range = data_vector.Get(5);
    // int less_range = data_vector.Get(-1);

    std::cout << "test end" << std::endl;
}

// int main() {
//     std::cout << "test" << std::endl;
// }