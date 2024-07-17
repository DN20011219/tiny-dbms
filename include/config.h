// Copyright (c) 2024 by dingning
//
// file  : config.h
// since : 2024-07-16
// desc  : Basic config of this project, contains the default vector type...

#ifndef VDBMS_CONFIG_H_
#define VDBMS_CONFIG_H_

#include "meta/vector.h"

namespace tiny_v_dbms {
    #define BASE_VECTOR Vector<int, double>
    #define default_length_size int 
    #define default_enum_type int
    enum column_type {NUMBER, VECTOR, VCHAR};
    enum column_index_type {NONE, FLAT};
}

#endif // VDBMS_CONFIG_H_