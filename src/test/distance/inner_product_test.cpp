#include <iostream>
// #include "../../../include/config.h"
#include "../../../include/distance/inner_product.h"

#define TestDistance InnerProduct<double>

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