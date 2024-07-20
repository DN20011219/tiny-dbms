// Copyright (c) 2024 by dingning
//
// file  : memory_ management.h
// since : 2024-07-18
// desc  : TODO.

#ifndef VDBMS_MEMORY_MANAGEMENT_H_
#define VDBMS_MEMORY_MANAGEMENT_H_

#include <mutex>
#include <list>
#include <iostream>

#include "../config.h"
#include "block_replacer.h"

using std::mutex;
using std::list;
using std::cout;
using std::endl; 


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

private:
    default_amount_type max_data_block_amount;  // store data block
    default_amount_type data_block_amount;      // maybe not used

    default_amount_type max_table_block_amount; // store table block

    MemoryBlock* data_memory_blocks;            // all blocks in this mm
    MemoryBlock* table_memory_blocks;           // all table blocks in this mm


    static MemoryManagement *m_SingleInstance;  // singleton object
    static std::mutex m_Mutex;

    // belows are replacer 
    list<MemoryBlock*> free_data_blocks;
    list<MemoryBlock*> using_data_blocks;
    
    list<MemoryBlock*> free_table_blocks;
    list<MemoryBlock*> using_table_blocks;


    BlockReplacer* data_replacer;
    BlockReplacer* table_replacer;



public:

    // get singleton MemoryManagement
    static MemoryManagement *&GetInstance()
    {
        if (m_SingleInstance == NULL) 
        {
            std::unique_lock<std::mutex> lock(m_Mutex); // 加锁
            if (m_SingleInstance == NULL)
            {
                m_SingleInstance = new (std::nothrow) MemoryManagement;
            }
        }

        return m_SingleInstance;
    }

    //释放单实例，进程退出时调用
    static void deleteInstance()
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        if (m_SingleInstance)
        {
            delete m_SingleInstance;
            m_SingleInstance = NULL;
        }
    }

private:
    
    MemoryManagement(const MemoryManagement &signal);

    const MemoryManagement &operator=(const MemoryManagement &signal);

    ~MemoryManagement()
    {
        free(data_memory_blocks);
        free(table_memory_blocks);
    }

    MemoryManagement()
    {
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

        AnalysizeSpaceUsed();

        // so can use different replace strategy in different type memory
        BlockReplacer replacer_one(data_memory_blocks, max_data_block_amount, free_data_blocks, using_data_blocks);
        data_replacer = &replacer_one;

        BlockReplacer replacer_two(table_memory_blocks, max_table_block_amount, free_table_blocks, using_table_blocks);
        table_replacer = &replacer_two;
    }

public:

    // analysize the memory space use
    void AnalysizeSpaceUsed()
    {
        cout << "______now spcace use analysize______" << endl;
        cout << "max_data_block_amount: " << max_data_block_amount << endl;
        cout << "max_table_block_amount: " << max_table_block_amount << endl;
        cout << "free_data_blocks: " << free_data_blocks.size() << endl;
        cout << "free_table_blocks: " << free_table_blocks.size() << endl;
        cout << "____________analysize end____________" << endl;
    }

    // read first table block of this db
    void ReadFirstTableBlockFromDisk(char*& block) 
    {

    }

    // use replacer to get a free table block
    void GetFreeTableBlock(char*& data)
    {
        if (!table_replacer->GetFreeBlock(data))
        {
            throw std::runtime_error("No enough space");
        }
    }

    void GetFreeDataBlock(char*& data)
    {
        if (!data_replacer->GetFreeBlock(data))
        {
            throw std::runtime_error("No enough space");
        }
    }

    void ReleaseBlock(char* data)
    {
        if (!data_replacer->ReleaseBlock(data))
        {
            table_replacer->ReleaseBlock(data);
        }
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