// Copyright (c) 2024 by dingning
//
// file  : column_table.h
// since : 2024-07-17
// desc  : Organize data by column, not by row.

#ifndef VDBMS_FLAT_INDEX_
#define VDBMS_FLAT_INDEX_

#include <string>

#include "../../config.h" 

namespace tiny_v_dbms {

using std::string; 

struct Column_Table 
{

    // information about the table
    string table_name;
    string table_type;
    default_amount_type column_size;                // amount of column
    string* column_name_array;                      // names of each column
    default_enum_type* column_type_array;           // type of each column
    default_enum_type* column_index_type_array;     // index type of each column

    // information about where to store actual data
    default_address_type* column_storage_address;   // where to store each column, is the address of first block

    /**
    * @brief return the space cost of this table header.
    */
    default_length_size GetSize() {
        return sizeof(string) * 2 + sizeof(default_amount_type) + (sizeof(string) + sizeof(default_enum_type) * 2 + sizeof(default_address_type)) * column_size;
    }
};

}

#endif // VDBMS_FLAT_INDEX_