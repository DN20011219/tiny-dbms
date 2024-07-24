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
        std::cout << "____MemoryManagement on deleted___" << std::endl;
        delete[] data_memory_blocks;
        delete[] table_memory_blocks;
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