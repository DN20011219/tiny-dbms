#include <iostream>
#include "../src/meta/vector.h"

#define TestVector Vector<int, int>

int main() {
    TestVector vector(10);
    int data[5] = {1, 2, 3, 4, 5};
    TestVector data_vector(data, 5);

    std::cout << "test begin" << std::endl;

    int first = data_vector.Get(0);
    int mid = data_vector.Get(2);
    int last = data_vector.Get(4);

    // int out_range = data_vector.Get(5);
    // int less_range = data_vector.Get(-1);
    
    std::cout << "test end" << std::endl;
}