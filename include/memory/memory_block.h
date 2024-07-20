// Copyright (c) 2024 by dingning
//
// file  : memory_block.h
// since : 2024-07-20
// desc  : TODO.

#ifndef VDBMS_MEMORY_MEMORY_BLOCK_H_
#define VDBMS_MEMORY_MEMORY_BLOCK_H_

#include <mutex>
#include "../config.h"


using std::mutex;

namespace tiny_v_dbms {

struct MemoryBlock 
{
    // default_address_type first_user_address;    // store the first user of this block, if user is empty, then clean this block
    default_amount_type user_amount;    // user amount, every user must add it when use one block, and sub it when end use. TODO: Can not solve cycle use.
    mutex access_control;               // lock one block, so other thread can not change or readit. TODO: 
    bool is_free;                       // if is free, then can use it.
    char* data;                         // the data this block store, it can be one of meta/block

    static default_length_size GetSize() {
        return sizeof(default_amount_type) + sizeof(mutex) + sizeof(bool) + BLOCK_SIZE;
    }
};

}

#endif // VDBMS_MEMORY_MEMORY_BLOCK_H_