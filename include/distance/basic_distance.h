// Copyright (c) 2024 by dingning
//
// file  : basic_distance.h
// since : 2024-07-16
// desc  : basic distance measure class, using a pointer to store the calculate function, and giving a unified interface to start calculate

#ifndef VDBMS_DISTANCE_BASIC_DISTANCE_H_
#define VDBMS_DISTANCE_BASIC_DISTANCE_H_

#include "../config.h"
#include "../meta/vector.h"

namespace tiny_v_dbms {

template<typename DATA_TYPE>
class BasicDistance {

public: 
  BasicDistance() {
    this->cal_function = DefaultCalFunction;
  }

  /**
  * @brief The entrance to calculate distance.
  * @param first_vector first vector.
  * @param second_vector second vector.
  */
  DATA_TYPE Cal(BASE_VECTOR & first_vector, BASE_VECTOR & second_vector) {
    return this->cal_function(first_vector, second_vector);
  }

  /**
  * @brief Set the cal function, open to children class.
  * @param cal_function calculate function.
  */
  void SetCalFunction(DATA_TYPE (*cal_function)(BASE_VECTOR & first, BASE_VECTOR & left)) {
    this->cal_function = cal_function;
  }

  /**
  * @brief The default function to calculate the distance between two vectors. Add up the differences in each digit.
  * @param first_vector first vector.
  * @param second_vector second vector.
  */
  static DATA_TYPE DefaultCalFunction(BASE_VECTOR & first_vector, BASE_VECTOR & second_vector) {

    assert(first_vector.GetLength() == second_vector.GetLength());

    DATA_TYPE result = 0;
    for (DATA_TYPE i = 0; i < first_vector.GetLength(); i++) {
      result += (first_vector.Get(i) - second_vector.Get(i));
    }

    return result;
  }

private:
  DATA_TYPE (*cal_function)(BASE_VECTOR & first, BASE_VECTOR & left); // pointer, store the function to calculate the distance between two vectors

}; 


}

#endif //VDBMS_DISTANCE_BASIC_DISTANCE_H_