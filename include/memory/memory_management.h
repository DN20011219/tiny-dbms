// Copyright (c) 2024 by dingning
//
// file  : memory_management.h
// since : 2024-07-18
// desc  : this is so called buffer_pool, but i called it as memory management.
// because this is the central that exchange data between disk and memory. It's not
// only control the pages store table data, but also control the pages store table header.

#ifndef VDBMS_MEMORY_MEMORY_MANAGEMENT_H_
#define VDBMS_MEMORY_MEMORY_MANAGEMENT_H_

#include <mutex>
#include <list>
#include <iostream>

#include "../config.h"
#include "replacer/block_base_replacer.h"
#include "memory_block_slot.h"

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

    MemoryBlockSlot* data_memory_blocks;            // all blocks in this mm
    MemoryBlockSlot* table_memory_blocks;           // all table blocks in this mm


    // belows are replacer 
    list<MemoryBlockSlot*> free_data_blocks;
    list<MemoryBlockSlot*> using_data_blocks;
    
    list<MemoryBlockSlot*> free_table_blocks;
    list<MemoryBlockSlot*> using_table_blocks;


    BlockBasicReplacer* data_replacer;
    BlockBasicReplacer* table_replacer;

private:

    static MemoryManagement *m_SingleInstance;  // singleton object
    static mutex m_Mutex;

    MemoryManagement();

    ~MemoryManagement()
    {
        std::cout << "____MemoryManagement on deleted___" << std::endl;
        delete[] data_memory_blocks;
        delete[] table_memory_blocks;
        delete[] data_replacer;
        delete[] table_replacer;
        std::cout << "____MemoryManagement has been deleted___" << std::endl;
    }

    MemoryManagement(const MemoryManagement &signal);

    const MemoryManagement &operator=(const MemoryManagement &signal);


public:

    // get singleton MemoryManagement
    static MemoryManagement *&GetInstance();

    // delete instance
    static void deleteInstance();

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

    // use replacer to get a free table block
    void GetFreeTableBlock(char*& data)
    {       
        // std::cout << "try get one block on mem" << std::endl;
        if (!table_replacer->GetFreeBlock(data))
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

};


}

#endif // VDBMS_MEMORY_MEMORY_MANAGEMENT_H_