// Copyright (c) 2024 by dingning
//
// file  : memory_block.h
// since : 2024-07-20
// desc  : basic block struct, designed to store some information to support replacer.
// this is the struct used in memory_management, it records more data about one block
// (such as used state), so it can provide information for block replacer

#ifndef VDBMS_MEMORY_MEMORY_BLOCK_SLOT_H_
#define VDBMS_MEMORY_MEMORY_BLOCK_SLOT_H_

#include <string>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include "../config.h"


using std::mutex;
using std::shared_mutex;
using std::condition_variable;
using std::string;

namespace tiny_v_dbms {

struct MemoryBlockSlot 
{
    string block_sign;                  // db_name + table_name + offset as one sign

    default_amount_type user_amount;    // user amount, every user must add it when use one block, and sub it when end use. TODO: Can not solve cycle use.
    shared_mutex read_or_write_mutex;            // read write mutex

    mutex unique_mutex;                 // mutex to get lock_condition
    condition_variable lock_condition;  // wait condition 

    bool is_free;                       // if is free, means this page has been release but not been clean.
    bool dirty_block;                   // if is one dirty page, it will write back when been clean.

    char* data;                         // the data this block store, it can be one of meta/block

    static default_length_size GetSize() {
        return sizeof(default_amount_type) + sizeof(mutex) + sizeof(bool) + BLOCK_SIZE;
    }
};

}

#endif // VDBMS_MEMORY_MEMORY_BLOCK_SLOT_H_