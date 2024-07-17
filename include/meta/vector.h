// Copyright (c) 2024 by dingning
//
// file  : vector.h
// since : 2024-07-16
// desc  : basic vector struct, support different type of data and length (such as int, size_t, long, unsigned int)

#ifndef VDBMS_META_VECTOR_H_
#define VDBMS_META_VECTOR_H_

#include <cassert>

namespace tiny_v_dbms {

template<typename LENGTH_TYPE, typename DATA_TYPE>
class Vector
{
private:
    unsigned int data_size;    // the space cost of one data item
    LENGTH_TYPE vector_length; // length of the vector
    DATA_TYPE* data;           // store the data of the vector

public:

    /**
    * @brief Init a empty vector using input length as space
    * @param length Length of this vector.
    */
    Vector(LENGTH_TYPE length) {
        data_size = sizeof(DATA_TYPE);
        vector_length = length;
        data = new DATA_TYPE[vector_length];
    }

    /**
    * @brief Init a vector, fill the data using input data.
    * @param data data to store.
    * @param length length of data.
    */
    Vector(const DATA_TYPE* data, LENGTH_TYPE length) {
        data_size = sizeof(DATA_TYPE);
        vector_length = length;
        this->data = new DATA_TYPE[length];
        memcpy(this->data, data, length * data_size); // Copy the data
    }
    
    inline DATA_TYPE Get(LENGTH_TYPE position) {
        assert(position >= 0 && position < vector_length);
        return *(data + position);
    }

    inline LENGTH_TYPE GetLength() {
        return vector_length;
    }

    ~Vector() {
        delete[] data;
    }
};

}

#endif // VDBMS_META_VECTOR_H_