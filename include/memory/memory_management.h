// Copyright (c) 2024 by dingning
//
// file  : memory_management.h
// since : 2024-07-18
// desc  : TODO.

#ifndef VDBMS_MEMORY_MEMORY_MANAGEMENT_H_
#define VDBMS_MEMORY_MEMORY_MANAGEMENT_H_

#include <mutex>
#include <list>
#include <iostream>

#include "../config.h"
#include "block_replacer.h"
#include "memory_block.h"

using std::mutex;
using std::list;
using std::cout;
using std::endl; 


namespace tiny_v_dbms {


class MemoryManagement {

private:
    default_amount_type max_data_block_amount;  // store data block
    default_amount_type data_block_amount;      // maybe not used

    default_amount_type max_table_block_amount; // store table block

    MemoryBlock* data_memory_blocks;            // all blocks in this mm
    MemoryBlock* table_memory_blocks;           // all table blocks in this mm


    // belows are replacer 
    list<MemoryBlock*> free_data_blocks;
    list<MemoryBlock*> using_data_blocks;
    
    list<MemoryBlock*> free_table_blocks;
    list<MemoryBlock*> using_table_blocks;


    BlockReplacer* data_replacer;
    BlockReplacer* table_replacer;

private:

    static MemoryManagement *m_SingleInstance;  // singleton object
    static mutex m_Mutex;

    MemoryManagement();

    ~MemoryManagement()
    {
        std::cout << "____MemoryManagement has been deleted___" << std::endl;
        delete[] data_memory_blocks;
        delete[] table_memory_blocks;
    }

    MemoryManagement(const MemoryManagement &signal);

    const MemoryManagement &operator=(const MemoryManagement &signal);


public:

    // get singleton MemoryManagement
    static MemoryManagement *&GetInstance();
    // {
    //     if (m_SingleInstance == NULL) 
    //     {
    //         std::unique_lock<std::mutex> lock(m_Mutex); // 加锁
    //         if (m_SingleInstance == NULL)
    //         {
    //             m_SingleInstance = new (std::nothrow) MemoryManagement;
    //         }
    //     }

    //     return m_SingleInstance;
    // }

    //释放单实例，进程退出时调用
    static void deleteInstance();
    // {
    //     std::unique_lock<std::mutex> lock(m_Mutex);
    //     if (m_SingleInstance)
    //     {
    //         delete m_SingleInstance;
    //         m_SingleInstance = NULL;
    //     }
    // }


    // MemoryManagement()
    // {   
    //     m_SingleInstance = nullptr;

    //     // init data blocks slots
    //     default_length_size total_space = (default_length_size) MEMORY_SIZE;
    //     max_data_block_amount = (total_space / 4 * 3) / MemoryBlock::GetSize(); // use max 3/4 of total space
    //     data_memory_blocks = new MemoryBlock[max_data_block_amount];
    //     for(int i = 0; i < max_data_block_amount; i++) {
    //         free_data_blocks.push_back(& data_memory_blocks[i]);
    //     }

    //     // init table blocks slots
    //     max_table_block_amount = (total_space / 8 * 1) / MemoryBlock::GetSize(); // use max 1/8 of total space
    //     table_memory_blocks = new MemoryBlock[max_table_block_amount];
    //     for(int i = 0; i < max_table_block_amount; i++) {
    //         free_table_blocks.push_back(& table_memory_blocks[i]);
    //     }

    //     AnalysizeSpaceUsed();

    //     // so can use different replace strategy in different type memory
    //     BlockReplacer replacer_one(data_memory_blocks, max_data_block_amount, free_data_blocks, using_data_blocks);
    //     data_replacer = &replacer_one;

    //     BlockReplacer replacer_two(table_memory_blocks, max_table_block_amount, free_table_blocks, using_table_blocks);
    //     table_replacer = &replacer_two;
    // }

    // analysize the memory space use
    void AnalysizeSpaceUsed()
    {
        cout << "______now memory spcace use analysize______" << endl;
        cout << "max_data_block_amount: " << max_data_block_amount << endl;
        cout << "max_table_block_amount: " << max_table_block_amount << endl;
        cout << "free_data_blocks: " << free_data_blocks.size() << endl;
        cout << "free_table_blocks: " << free_table_blocks.size() << endl;
        cout << "______________analysize end_______________" << endl;
    }

    // read first table block of this db
    void ReadFirstTableBlockFromDisk(char*& block) 
    {

    }

    // use replacer to get a free table block
    void GetFreeTableBlock(char*& data)
    {       
        std::cout << "try get one block on mem" << std::endl;
        if (!table_replacer->GetFreeBlock(data))
        {   
            throw std::runtime_error("No enough space");
        }
    }

    // void GetFreeDataBlock(char*& data)
    // {
    //     if (!data_replacer->GetFreeBlock(data))
    //     {
    //         throw std::runtime_error("No enough space");
    //     }
    // }

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

#endif // VDBMS_MEMORY_MEMORY_MANAGEMENT_H_