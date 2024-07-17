// Copyright (c) 2024 by dingning
//
// file  : inner_product.h
// since : 2024-07-17
// desc  : 
#ifndef VDBMS_DISTANCE_INNER_PRODUCT_H_
#define VDBMS_DISTANCE_INNER_PRODUCT_H_

#include "basic_distance.h"

namespace tiny_v_dbms {

template<typename DATA_TYPE>
class InnerProduct : public BasicDistance<DATA_TYPE> {

public:

    InnerProduct() {
        this->SetCalFunction(InnerProductFunction);
    }

    /**
    * @brief The function to calculate the inner product distance between two vectors. 
    * @param first_vector first vector.
    * @param second_vector second vector.
    */
    static DATA_TYPE InnerProductFunction(BASE_VECTOR & first_vector, BASE_VECTOR & second_vector) {

        assert(first_vector.GetLength() == second_vector.GetLength());

        DATA_TYPE result = 0;
        for (DATA_TYPE i = 0; i < first_vector.GetLength(); i++) {
            result += (first_vector.Get(i) * second_vector.Get(i));
        }
    
        return result;
    }
    
};

}
#endif // VDBMS_DISTANCE_INNER_PRODUCT_H_