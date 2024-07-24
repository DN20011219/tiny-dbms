// Copyright (c) 2024 by dingning
//
// file  : column_array.h
// since : 2024-07-24
// desc  : A array struct stores a collection of columns.

#ifndef VDBMS_META_COLUMN_COLUMN_ARRAY_H_
#define VDBMS_META_COLUMN_COLUMN_ARRAY_H_

#include <string>

#include "../../config.h" 

namespace tiny_v_dbms {

struct Columns {
    std::string* column_name_array;                       // names of each column
    default_enum_type* column_type_array;                 // type of each column
    default_length_size* column_length_array;             // space cost of each column
    default_enum_type* column_index_type_array;           // index type of each column
    default_address_type* column_storage_address_array;   // where to store each column, is the address of first block number in table data file
};

}

#endif // VDBMS_META_COLUMN_COLUMN_ARRAY_H_