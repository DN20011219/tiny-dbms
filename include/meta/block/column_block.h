// Copyright (c) 2024 by dingning
//
// file  : column_block.h
// since : 2024-07-18
// desc  : Basic unit to read from disk or write to disk. This type block means 
// each block only store one type of field (same named fields from different t-
// ables are regarded as different fields), and data is stored one besides ano-
// ther one.

#ifndef VDBMS_META_BLOCK_COLUMN_BLOCK_H_
#define VDBMS_META_BLOCK_COLUMN_BLOCK_H_

#include <string>

#include "../../config.h" 

namespace tiny_v_dbms {

using std::string; 

struct Column_Block 
{

public:

    // information about this block
    default_length_size field_length;       // length of per field
    default_length_size field_data_nums;    // amount of data
    default_address_type next_block_pointer;// store a pointer to next block, if has next block

    Column_Block() {
        assert(default_pointer_size == sizeof(next_block_pointer));     // must be 32bit device to work
    }
};

}

#endif // VDBMS_META_BLOCK_COLUMN_BLOCK_H_