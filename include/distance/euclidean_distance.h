// Copyright (c) 2024 by dingning
//
// file  : euclidean_distance.h
// since : 2024-07-16
// desc  : 
#ifndef VDBMS_DISTANCE_EUCLIDEAN_DISTANCE_H_
#define VDBMS_DISTANCE_EUCLIDEAN_DISTANCE_H_

#include <cmath>
#include "basic_distance.h"

namespace tiny_v_dbms {

template<typename DATA_TYPE>
class EuclideanDistance : public BasicDistance<DATA_TYPE> {

public:

    EuclideanDistance() {
        this->SetCalFunction(EuclideanDistanceFunction);
    }

    /**
    * @brief The function to calculate the Euclidean distance between two vectors. 
    * @param first_vector first vector.
    * @param second_vector second vector.
    */
    static DATA_TYPE EuclideanDistanceFunction(BASE_VECTOR & first_vector, BASE_VECTOR & second_vector) {

        assert(first_vector.GetLength() == second_vector.GetLength());

        DATA_TYPE result = 0;
        for (DATA_TYPE i = 0; i < first_vector.GetLength(); i++) {

            DATA_TYPE part_result = (first_vector.Get(i) - second_vector.Get(i));
            part_result *= part_result;

            result += part_result;
        }
    
        return sqrt(result);
    }
    
};

}
#endif // VDBMS_DISTANCE_EUCLIDEAN_DISTANCE_H_