// Copyright (c) 2024 by dingning
//
// file  : memory_ management.h
// since : 2024-07-18
// desc  : TODO.

#ifndef VDBMS_MEMORY_MANAGEMENT_H_
#define VDBMS_MEMORY_MANAGEMENT_H_

#include <mutex>
#include <list>

#include "../config.h"

using std::mutex;
using std::list;

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

class MemoryManagement {
    default_amount_type max_data_block_amount;  // store data block
    default_amount_type data_block_amount;      // maybe not used

    default_amount_type max_table_block_amount; // store table block

    MemoryBlock* data_memory_blocks;            // all blocks in this mm
    MemoryBlock* table_memory_blocks;           // all table blocks in this mm

    list<MemoryBlock*> free_data_blocks;
    list<MemoryBlock*> using_data_blocks;
    
    list<MemoryBlock*> free_table_blocks;
    list<MemoryBlock*> using_table_blocks;

public:

    MemoryManagement() {

        // init data blocks slots
        default_length_size total_space = (default_length_size) MEMORY_SIZE;
        max_data_block_amount = (total_space / 4 * 3) / MemoryBlock::GetSize(); // use max 3/4 of total space
        data_memory_blocks = new MemoryBlock[max_data_block_amount];
        for(int i = 0; i < max_data_block_amount; i++) {
            free_data_blocks.push_back(& data_memory_blocks[i]);
        }

        // init table blocks slots
        max_table_block_amount = (total_space / 8 * 1) / MemoryBlock::GetSize(); // use max 1/8 of total space
        table_memory_blocks = new MemoryBlock[max_table_block_amount];
        for(int i = 0; i < max_table_block_amount; i++) {
            free_table_blocks.push_back(& table_memory_blocks[i]);
        }

    }


    // read first table block of this db
    void ReadFirstTableBlockFromDisk(char*& block) {
        // 
    }



    // void ExtendBlockSpace() {
    //     default_amount_type cache = block_amount * block_amount;
    //     cache = cache < max_block_amount ? cache : max_block_amount; // extends space = now space * 2

    //     MemoryBlock* cache_blocks = new MemoryBlock[block_amount];
    //     memcpy(cache_blocks, memory_blocks, block_amount * MemoryBlock::GetSize()); // Copy the blocks data

    //     memory_blocks = cache_blocks;
    // }


};


}

#endif // VDBMS_MEMORY_MANAGEMENT_H_