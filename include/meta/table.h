// Copyright (c) 2024 by dingning
//
// file  : table.h
// since : 2024-07-17
// desc  : store data by column, not by row.

#ifndef VDBMS_FLAT_INDEX_
#define VDBMS_FLAT_INDEX_

#include <string>

#include "../config.h" 

namespace tiny_v_dbms {

using std::string; 

struct table {

    // information about the table
    string table_name;
    string table_type;
    default_length_size column_size;
    string* column_name_array;
    default_enum_type* column_type_array;
    default_enum_type* column_index_type_array;

    // information about where and who to store data
    string meta_file_dir;
    string data_file_dir;
};

}

#endif // VDBMS_FLAT_INDEX_